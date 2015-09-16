using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FractalRenderer.Colorizers
{
    public abstract class Colorizer
    {
        public abstract Bitmap DrawBitmap(OrbitMap m);

        public abstract Bitmap DrawBitmap(OrbitMap m, RenderParameters rp);

        public abstract Bitmap DrawBitmap(OrbitMap m, RenderParameters rp, Constraints[] orbitConstraints);
    }
}
