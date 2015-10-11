using System.Threading;

namespace FractalRenderer
{
    public abstract class Calculator
    {
        public delegate void RenderCompleteCallback(OrbitMap image);

        public abstract OrbitMap Calculate(CalculationOptions options);

        public virtual void RenderAsync(CalculationOptions options, RenderCompleteCallback callback)
        {
            ThreadPool.QueueUserWorkItem((o) =>
            {
                CalculationOptions op = (o as object[])[0] as CalculationOptions;
                RenderCompleteCallback cb = (o as object[])[1] as RenderCompleteCallback;

                OrbitMap calc = Calculate(op);
                cb(calc);
            }, new object[] { options, callback });
        }
    }
}
