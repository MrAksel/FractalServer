using System.Drawing;

namespace FractalRenderer.Colorizers
{
    public abstract class Colorizer
    {
        public abstract Bitmap DrawBitmap(OrbitMap m, RenderOptions rp, CalculationOptions co, Constraints[] orbitConstraints);
    }
}
