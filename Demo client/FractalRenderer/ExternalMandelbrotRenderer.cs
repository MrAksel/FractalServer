using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace FractalRenderer
{
    public class ExternalMandelbrotCalculator : Calculator
    {
        public HostSelector HostSelector { get; set; }

        public override OrbitMap Calculate(CalculationOptions options)
        {
            // Needs finetuning - a fast internet connection would benefit from smaller tasks (better utilization of computing cores),
            // but a slow one might become too slow from the overhead
            Queue<CalculationOptions> tasks = new Queue<CalculationOptions>(RenderTaskHelper.Split(options, 8 * HostSelector.EffectiveWorkers));

            OrbitMap full = new OrbitMap();
            List<RendererConnection> connections = new List<RendererConnection>();

            while (tasks.Count > 0) // While there are still tasks left
            {
                // Distribute tasks to available renderers
                DistributeTasks(HostSelector, tasks, connections);

                // While we have running render tasks
                while (connections.Count > 0)
                {
                    // Wait for one task to finish
                    WaitHandle[] handles = connections.Select(c => c.WaitHandle).ToArray();
                    int h = WaitHandle.WaitAny(handles);

                    RendererConnection conn = connections[h];
                    connections.RemoveAt(h);

                    HostSelector.ConnectionCompleted(conn);

                    if (conn.Success)
                    { 
                        // If it was a success, extend the OrbitMap with these results
                        full.Extend(conn.Result);
                    }
                    else
                    {
                        // TODO error handling, retry?
                        tasks.Enqueue(conn.Task);             // Re-enqueue it until it succeeds (maybe have to do something about that)
                    }

                    // Got an available renderer - fill it with a task
                    DistributeTasks(HostSelector, tasks, connections);
                }
            }
            return full; // Return the combined orbit map
        }

        private void DistributeTasks(HostSelector sel, Queue<CalculationOptions> rendertasks, List<RendererConnection> connections)
        {
            while (rendertasks.Count > 0 && sel.AvailableWorkers > 0)
            {
                CalculationOptions opt = rendertasks.Dequeue();
                RendererConnection conn = sel.GetConnection();
                conn.BeginRenderAsync(opt);
                connections.Add(conn);
            }
        }
    }
}
