using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Threading;

namespace FractalRenderer
{
    internal class RendererSelector
    {
        private object threadlock;
        private ManualResetEvent resevent;

        private List<IPEndPoint> renderHosts;
        private List<IPEndPoint> availableHosts;
        private List<RendererConnection> waiting;

        internal RendererSelector() : this(new List<IPEndPoint>())
        {

        }

        internal RendererSelector(IEnumerable<IPEndPoint> renderHosts)
        {
            this.threadlock = new object();
            this.resevent = new ManualResetEvent(false);

            this.renderHosts = renderHosts.ToList();
            this.availableHosts = new List<IPEndPoint>(this.renderHosts);
            this.waiting = new List<RendererConnection>();
        }

        internal List<IPEndPoint> Hosts
        {
            get
            {
                return renderHosts;
            }
            set
            {
                renderHosts = value;
            }
        }

        public int AvailableHosts
        {
            get
            {
                lock (threadlock)
                    return availableHosts.Count;
            }
        }

        internal RendererConnection GetConnection()
        {
            IPEndPoint ep;
            while (true)
            {
                Monitor.Enter(threadlock);
                if (availableHosts.Count > 0)
                {
                    // TODO make a struct for RenderHost with a priority, and sort by priority
                    // availableHosts.Sort(); 
                    ep = availableHosts[0]; // Dequeue an available host
                    availableHosts.RemoveAt(0);

                    Monitor.Exit(threadlock);
                    break;
                }
                else
                {
                    Monitor.Exit(threadlock);
                    resevent.WaitOne(); // Wait for one to be put in the list
                }
            }

            RendererConnection c = new RendererConnection(ep);
            c.CompletedEvent += (conn) =>
            {
                /*
                if (conn.Success) // Adjust priority based on success or error
                    conn.Host.Priority = conn.Host.Priority + 1;
                else
                    conn.Host.Priority = conn.Host.Priority - 1;
                */

                lock (threadlock)
                    availableHosts.Add(conn.Host); // Put the host back in the queue of available hosts
                resevent.Set(); // Signal that there is a new one available
            };
            return c;
        }

        internal void DecreasePopularity(IPEndPoint host)
        {
            // TODO sort RenderHosts by priority (see above)
        }
    }
}