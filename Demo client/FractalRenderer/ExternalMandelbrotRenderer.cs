using MiscUtil.Conversion;
using MiscUtil.IO;
using Numerics;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Numerics;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace FractalRenderer
{
    public class ExternalMadelbrotRenderer : Renderer
    {
        public IEnumerable<IPEndPoint> RenderHosts { get; set; }

        public override OrbitMap Render(RenderOptions options)
        {
            RendererSelector sel = new RendererSelector(RenderHosts);
            Queue<RenderOptions> rendertasks = new Queue<RenderOptions>(RenderTaskHelper.Split(options, 2 * sel.Hosts.Count));

            OrbitMap full = new OrbitMap();
            List<RendererConnection> connections = new List<RendererConnection>();

            while (rendertasks.Count > 0)
            {
                // Distribute tasks to available renderers
                DistributeTasks(sel, rendertasks, connections);

                // While we have running render tasks
                while (connections.Count > 0)
                {
                    // Wait for one task to finish
                    WaitHandle[] handles = connections.Select(c => c.WaitHandle).ToArray();
                    int h = WaitHandle.WaitAny(handles);

                    RendererConnection conn = connections[h];
                    if (conn.Success)
                    { 
                        // If it was a success, extend the OrbitMap with these results
                        full.Extend(conn.Result);
                    }
                    else
                    {
                        // TODO error handling, retry?
                        sel.DecreasePopularity(conn.Host); // Make it less likely to be selected for next render task
                        rendertasks.Enqueue(conn.Task);    // Re-enqueue it until it succeeds (maybe have to do something about that)
                    }

                    // Got an available renderer - fill it with a task
                    DistributeTasks(sel, rendertasks, connections);
                }
            }
            return full; // Return the combined orbit map
        }

        private void DistributeTasks(RendererSelector sel, Queue<RenderOptions> rendertasks, List<RendererConnection> connections)
        {
            while (rendertasks.Count > 0 && sel.AvailableHosts > 0)
            {
                RenderOptions opt = rendertasks.Dequeue();
                RendererConnection conn = sel.GetConnection();
                conn.BeginRenderAsync(opt);
                connections.Add(conn);
            }
        }
    }
}
