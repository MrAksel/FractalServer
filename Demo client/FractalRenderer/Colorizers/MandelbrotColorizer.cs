using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Runtime.InteropServices;

namespace FractalRenderer.Colorizers
{
    public class MandelbrotColorizer : Colorizer
    {
#if UNSAFE
        public unsafe override Bitmap DrawBitmap(OrbitMap m, RenderOptions rp, Constraints[] orbitConstraints)
        {
            Bitmap b = new Bitmap(rp.CanvasWidth, rp.CanvasHeight, PixelFormat.Format32bppRgb);
            BitmapData bd = b.LockBits(new Rectangle(0, 0, rp.CanvasWidth, rp.CanvasHeight), ImageLockMode.ReadWrite, PixelFormat.Format32bppRgb);

            uint maxIter = m.Max(o => o.Iterations);

            uint* pixels = (uint*)bd.Scan0.ToPointer();
            foreach (Orbit o in m)
            {
                if (o.X >= 0 && o.X < b.Width && 
                    o.Y >= 0 && o.Y < b.Height)
                {
                    uint gray = (o.Iterations * 255) / maxIter;
                    pixels[o.Y * bd.Stride + o.X] = (gray << 24) | (gray << 16) | (gray << 8) | gray;
                }
            }

            b.UnlockBits(bd);
            return b;
        }
#else
        public override Bitmap DrawBitmap(OrbitMap m, RenderOptions rp, Constraints[] orbitConstraints)
        {
            Bitmap b = new Bitmap(rp.CanvasWidth, rp.CanvasHeight, PixelFormat.Format32bppRgb);
            BitmapData bd = b.LockBits(new Rectangle(0, 0, rp.CanvasWidth, rp.CanvasHeight), ImageLockMode.ReadWrite, PixelFormat.Format32bppRgb);

            uint maxIter = m.Max(o => o.Iterations);

            foreach (Orbit o in m)
            {
                if (o.X >= 0 && o.X < b.Width &&
                    o.Y >= 0 && o.Y < b.Height)
                {
                    uint gray = (uint)((ulong)o.Iterations * 255) / maxIter;
                    Marshal.WriteInt32(bd.Scan0 + o.Y * bd.Stride + o.X,
                                       unchecked ((int)((gray << 24) | (gray << 16) | (gray << 8) | gray)));
                }
            }

            b.UnlockBits(bd);
            return b;
        }
#endif
    }
}
