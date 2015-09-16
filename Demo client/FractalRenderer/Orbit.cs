using System;
using Numerics;
using System.Collections.Generic;

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

        public Orbit(int x, int y, BigComplex c, uint iter, uint orbitStart, uint orbl)
        {
            this.x = x;
            this.y = y;
            this.c = c;

            this.iterations = iter;
            this.orbitOffset = orbitStart;
            this.orbitLength = orbl;

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