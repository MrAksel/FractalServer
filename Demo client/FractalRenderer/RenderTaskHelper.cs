using System;
using System.Collections.Generic;

namespace FractalRenderer
{
    internal static class RenderTaskHelper
    {
        internal static CalculationOptions[] Split(CalculationOptions options, int tasks)
        {
            if (tasks < 1)
                throw new ArgumentOutOfRangeException();
            return Split(options, (uint)tasks);
        }

        internal static CalculationOptions[] Split(CalculationOptions options, uint tasks)
        {
            Queue<CalculationOptions> t1 = new Queue<CalculationOptions>();
            Queue<CalculationOptions> t2 = new Queue<CalculationOptions>();

            t1.Enqueue(options);

            tasks = (uint)Math.Pow(4, Math.Ceiling(Math.Log(tasks) / Math.Log(4)));

            while (tasks > 0)
            {
                while (t1.Count > 0)
                {
                    CalculationOptions t = t1.Dequeue();
                    foreach (CalculationOptions spl in SplitInFour(t))
                    {
                        t2.Enqueue(spl);
                    }
                }
                tasks /= 4;
                t1 = t2;
                t2 = new Queue<CalculationOptions>();
            }

            return t1.ToArray();
        }

        internal static CalculationOptions[] SplitInFour(CalculationOptions options)
        {
            CalculationOptions[] opt = new CalculationOptions[4];

            opt[0] = options.Duplicate();
            opt[0].XSkip *= 2;
            opt[0].YSkip *= 2;

            opt[1] = options.Duplicate();
            opt[1].XSkip *= 2;
            opt[1].YSkip *= 2;
            opt[1].XOff += options.XSkip;

            opt[2] = options.Duplicate();
            opt[2].XSkip *= 2;
            opt[2].YSkip *= 2;
            opt[2].YSkip += options.YSkip;

            opt[3] = options.Duplicate();
            opt[3].XSkip *= 2;
            opt[3].YSkip *= 2;
            opt[3].XOff += options.XSkip;
            opt[3].YOff += options.YSkip;

            return opt;
        }
    }
}