using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace FractalRenderer
{
    public class HostSelector
    {
        private object threadlock;
        private ManualResetEvent resevent;

        private Dictionary<RenderHost, int> renderHosts;
        private Dictionary<RendererConnection, RenderHost> waiting;

        public HostSelector() : this(new List<RenderHost>())
        {

        }

        public HostSelector(IEnumerable<RenderHost> renderHosts)
        {
            this.threadlock = new object();
            this.resevent = new ManualResetEvent(false);

            this.renderHosts = new Dictionary<RenderHost, int>();
            foreach (RenderHost h in renderHosts)
                this.renderHosts.Add(h, h.NumWorkers);

            this.waiting = new Dictionary<RendererConnection, RenderHost>();
        }

        public void AddHost(RenderHost host)
        {
            lock (threadlock)
                renderHosts.Add(host, host.NumWorkers);
            resevent.Set();
        }

        internal List<RenderHost> Hosts
        {
            get
            {
                lock (threadlock)
                    return renderHosts.Keys.ToList();
            }
        }

        public int EffectiveWorkers
        {
            get
            {
                lock (threadlock)
                    return renderHosts.Sum(h => h.Key.NumWorkers);
            }
        }

        public int AvailableWorkers
        {
            get
            {
                lock (threadlock)
                    return renderHosts.Sum(h => h.Value);
            }
        }

        internal RendererConnection GetConnection()
        {
            RenderHost host;
            while (true)
            {
                Monitor.Enter(threadlock);
                if (AvailableWorkers > 0)
                {
                    host = renderHosts.Where(p => p.Value > 0)                  // Select a host with available workers
                                      .OrderByDescending(p => p.Key.Priority)   // Order by priority
                                      .First().Key;                             // Take the first and best

                    Monitor.Exit(threadlock);
                    break;
                }
                else
                {
                    Monitor.Exit(threadlock);
                    resevent.WaitOne(); // Wait for one to be put in the list
                }
            }

            RendererConnection c = new RendererConnection(host.EndPoint);

            lock (threadlock)
            {
                renderHosts[host]--;    // Decease number of available workers on this host
                waiting.Add(c, host);   // Add the connection to the list of running ops
            }

            return c;
        }

        internal void ConnectionCompleted(RendererConnection conn)
        {
            RenderHost host;
            lock (threadlock)
            {
                host = waiting[conn];    // Retrieve the host associated with this connection
                waiting.Remove(conn);
            }

            if (conn.Success) // Adjust priority based on success or error
                host.Priority++;
            else
                host.Priority--;

            lock (threadlock)
                renderHosts[host]++; // Increase the number of available workers on this host again

            resevent.Set(); // Signal that there is a new worker available
        }
    }
}