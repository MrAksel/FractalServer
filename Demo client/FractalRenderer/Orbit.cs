using System;
using Numerics;

// TODO Create a real (ha-ha) BigComplex type
using BigComplex = System.Tuple<Numerics.BigRational, Numerics.BigRational>;

namespace FractalRenderer
{
    public class Orbit
    {
        private int x;
        private int y;
        private BigComplex c;

        private uint iterations;
        private uint orbitOffset;
        private uint orbitLength;

        private BigComplex[] points;
        private uint currentindex;

        public int X
        {
            get
            {
                return x;
            }

            set
            {
                x = value;
            }
        }

        public int Y
        {
            get
            {
                return y;
            }

            set
            {
                y = value;
            }
        }

        public BigComplex StartingPoint
        {
            get
            {
                return c;
            }

            set
            {
                c = value;
            }
        }

        public uint Iterations
        {
            get
            {
                return iterations;
            }

            set
            {
                iterations = value;
            }
        }

        public uint OrbitOffset
        {
            get
            {
                return orbitOffset;
            }

            set
            {
                orbitOffset = value;
            }
        }

        public uint OrbitLength
        {
            get
            {
                return orbitLength;
            }

            set
            {
                orbitLength = value;
            }
        }

        public Orbit(int x, int y, BigComplex c, uint iter, uint orbitStart, uint orbl)
        {
            this.X = x;
            this.Y = y;
            this.StartingPoint = c;

            this.Iterations = iter;
            this.OrbitOffset = orbitStart;
            this.OrbitLength = orbl;

            this.points = new BigComplex[orbl];
            this.currentindex = 0;
        }

        internal void Add(BigRational re, BigRational im)
        {
            if (currentindex == points.LongLength) // Have to expand array
            {
                long newsize = points.LongLength == 0 ? 1 : points.LongLength * 2;
                BigComplex[] newarr = new BigComplex[newsize];
                Array.Copy(points, newarr, currentindex);
                points = newarr;
            }
            points[currentindex++] = new BigComplex(re, im);
        }
    }
}