using System.Net;

namespace FractalRenderer
{
    public class RenderHost
    {
        public IPEndPoint EndPoint { get; internal set; }
        public int Priority { get; internal set; }
        public int NumWorkers { get; internal set; }

        public RenderHost(IPEndPoint ep, int num_workers)
        {
            EndPoint = ep;
            Priority = 0;
            NumWorkers = num_workers;
        }
    }
}