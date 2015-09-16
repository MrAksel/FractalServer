using System.Drawing;
using System.Threading;

namespace FractalRenderer
{
    public abstract class Renderer
    {
        public delegate void RenderCompleteCallback(OrbitMap image);

        public abstract OrbitMap Render(RenderOptions options);

        public virtual void RenderAsync(RenderOptions options, RenderCompleteCallback callback)
        {
            ThreadPool.QueueUserWorkItem((o) =>
            {
                RenderOptions op = (o as object[])[0] as RenderOptions;
                RenderCompleteCallback cb = (o as object[])[1] as RenderCompleteCallback;

                OrbitMap calc = Render(op);
                cb(calc);
            }, new object[] { options, callback });
        }
    }
}
