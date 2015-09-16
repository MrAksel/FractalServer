using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FractalRenderer.Colorizers
{
    public class MandelbrotColorizer : Colorizer
    {
        public override Bitmap DrawBitmap(OrbitMap m)
        {
            return DrawBitmap(m, RenderParameters.Empty, null);
        }

        public override Bitmap DrawBitmap(OrbitMap m, RenderParameters rp)
        {
            return DrawBitmap(m, rp, null);
        }

        public override Bitmap DrawBitmap(OrbitMap m, RenderParameters rp, Constraints[] orbitConstraints)
        {
            throw new NotImplementedException();
        }
    }
}
