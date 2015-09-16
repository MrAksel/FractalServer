using System;

namespace FractalRenderer
{
    internal static class RenderTaskHelper
    {
        internal static RenderOptions[] Interlace(RenderOptions options, uint passes)
        {
            throw new NotImplementedException();
        }

        internal static RenderOptions[] Split(RenderOptions options, int tasks)
        {
            if (tasks < 1)
                throw new ArgumentOutOfRangeException();
            return Split(options, (uint)tasks);
        }

        internal static RenderOptions[] Split(RenderOptions options, uint tasks)
        {
            throw new NotImplementedException();
        }
    }
}