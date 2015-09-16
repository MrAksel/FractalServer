using System;
using System.Collections;
using System.Collections.Generic;

namespace FractalRenderer
{
    public class OrbitMap : IEnumerable<Orbit>
    {
        private List<Orbit> orbits;

        public OrbitMap()
        {
            orbits = new List<Orbit>();
        }

        internal void Extend(OrbitMap other)
        {
            this.orbits.AddRange(other.orbits);
        }

        internal void AddOrbit(Orbit o)
        {
            orbits.Add(o);
        }


        public IEnumerator<Orbit> GetEnumerator()
        {
            return ((IEnumerable<Orbit>)orbits).GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return ((IEnumerable<Orbit>)orbits).GetEnumerator();
        }
    }
}