using MiscUtil.Conversion;
using MiscUtil.IO;
using Numerics;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Numerics;

namespace FractalRenderer
{
    public class RenderOptions : INotifyPropertyChanged
    {
        uint iteration_count;
        uint interlacing_pass;

        int width, height;

        int xOff, xSkip, xMax;
        int yOff, ySkip, yMax;

        uint orbit_start;
        uint orbit_length;
        uint skip_primary_bulbs;

        BigRational minRe, maxRe, minIm, maxIm;

        public uint OrbitStart
        {
            get
            {
                return orbit_start;
            }
            set
            {
                orbit_start = value;
                NotifyPropertyChanged("OrbitStart");
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
                NotifyPropertyChanged("OrbitLength");
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
                NotifyPropertyChanged("Width");
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
                NotifyPropertyChanged("Height");
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
                NotifyPropertyChanged("MinRe");
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
                NotifyPropertyChanged("MaxRe");
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
                NotifyPropertyChanged("MinIm");
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
                NotifyPropertyChanged("MaxIm");
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
                NotifyPropertyChanged("IterationCount");
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
                skip_primary_bulbs = (value == true ? 1U : 0U);
            }
        }

        public uint InterlacingPass
        {
            get
            {
                return interlacing_pass;
            }
            set
            {
                interlacing_pass = value;
            }
        }

        public uint NumInterlacingPasses
        {
            get;
            set;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string prop)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(prop));
        }
    }
}
