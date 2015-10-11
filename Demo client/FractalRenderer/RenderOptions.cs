using Numerics;

namespace FractalRenderer
{
    public class CalculationOptions
    {
        uint iteration_count;

        int width, height;

        int xOff, xSkip, xMax;
        int yOff, ySkip, yMax;

        uint orbit_start;
        uint orbit_length;
        uint skip_primary_bulbs;

        BigRational minRe, maxRe, minIm, maxIm;

        public CalculationOptions(int width, int height, uint iter)
        {
            this.width = xMax = width;
            this.height = yMax = height;
            this.xSkip = 1;
            this.ySkip = 1;
            this.iteration_count = iter;
            this.orbit_length = iter;
            this.skip_primary_bulbs = 0U;

            this.minRe = new BigRational(-5, 2);    // -2.5 to
            this.maxRe = new BigRational(3, 2);     //  1.5
            this.minIm = new BigRational(-3, 2);    // -1.5 to
            this.maxIm = new BigRational(3, 2);     //  1.5
        }

        internal CalculationOptions Duplicate()
        {
            return new CalculationOptions(width, height, iteration_count)
            {
                XSkip = xSkip,
                YSkip = ySkip,
                OrbitLength = orbit_length,
                BulbChecking = BulbChecking,
                MinRe = minRe,
                MaxRe = maxRe,
                MinIm = minIm,
                MaxIm = maxIm
            };
        }

        public uint OrbitStart
        {
            get
            {
                return orbit_start;
            }
            set
            {
                orbit_start = value;
            }
        }

        public uint OrbitLength
        {
            get
            {
                return orbit_length;
            }
            set
            {
                orbit_length = value;
            }
        }

        public int Width
        {
            get
            {
                return width;
            }

            set
            {
                width = value;
            }
        }

        public int Height
        {
            get
            {
                return height;
            }

            set
            {
                height = value;
            }
        }

        public BigRational MinRe
        {
            get
            {
                return minRe;
            }

            set
            {
                minRe = value;
            }
        }

        public BigRational MaxRe
        {
            get
            {
                return maxRe;
            }

            set
            {
                maxRe = value;
            }
        }

        public BigRational MinIm
        {
            get
            {
                return minIm;
            }

            set
            {
                minIm = value;
            }
        }

        public BigRational MaxIm
        {
            get
            {
                return maxIm;
            }

            set
            {
                maxIm = value;
            }
        }

        public uint IterationCount
        {
            get
            {
                return iteration_count;
            }
            set
            {
                iteration_count = value;
            }
        }

        public int XOff
        {
            get
            {
                return xOff;
            }
            set
            {
                xOff = value;
            }
        }

        public int XSkip
        {
            get
            {
                return xSkip;
            }
            set
            {
                xSkip = value;
            }
        }

        public int XMax
        {
            get
            {
                return xMax;
            }
            set
            {
                xMax = value;
            }
        }

        public int YOff
        {
            get
            {
                return yOff;
            }
            set
            {
                yOff = value;
            }
        }

        public int YSkip
        {
            get
            {
                return ySkip;
            }
            set
            {
                ySkip = value;
            }
        }

        public int YMax
        {
            get
            {
                return yMax;
            }
            set
            {
                yMax = value;
            }
        }

        public bool BulbChecking
        {
            get
            {
                return skip_primary_bulbs != 0U;
            }
            set
            {
                skip_primary_bulbs = (value ? 1U : 0U);
            }
        }
    }
}
