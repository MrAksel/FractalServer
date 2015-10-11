using MiscUtil.Conversion;
using MiscUtil.IO;
using Numerics;
using System;
using System.Net;
using System.Net.Sockets;
using System.Numerics;
using System.Threading;
using BigComplex = System.Tuple<Numerics.BigRational, Numerics.BigRational>;

namespace FractalRenderer
{
    internal class RendererConnection
    {
        private Thread worker;

        public bool Success { get; internal set; }
        public OrbitMap Result { get; internal set; }
        public IPEndPoint Host { get; internal set; }
        public CalculationOptions Task { get; internal set; }
        public WaitHandle WaitHandle { get; internal set; }

        internal RendererConnection(IPEndPoint host)
        {
            Success = false;

            Host = host;
            WaitHandle = new ManualResetEvent(false);
        }

        internal void BeginRenderAsync(CalculationOptions opt)
        {
            Task = opt;
            worker = new Thread(Work);
            worker.Start(opt);
        }

        private void Work(object arg)
        {
            CalculationOptions opt = arg as CalculationOptions;
            NetworkStream ns = null;
            Success = false;
            try
            {
                ns = TcpHelper.OpenStream(Host, 1);

                byte[] magic = new byte[4];
                ns.Read(magic, 0, 4);
                uint v = BitConverter.ToUInt32(magic, 0);

                bool be = (v == 0xAFDE0000); // Renderer sends 00 00 DE AF, if its reversed all is in big endian

                EndianBitConverter ec;
                if (be) ec = new BigEndianBitConverter();
                else ec = new LittleEndianBitConverter();

                using (EndianBinaryWriter bw = new EndianBinaryWriter(ec, ns))
                using (EndianBinaryReader br = new EndianBinaryReader(ec, ns))
                {
                    bw.Write((uint)0x0000FEED);

                    // Write rational coordinates
                    foreach (BigRational r in new[] { opt.MinRe, opt.MaxRe, opt.MinIm, opt.MaxIm })
                    {
                        int sign = r.Numerator.Sign;
                        byte[] num = BigInteger.Abs(r.Numerator).ToByteArray();
                        bw.Write(sign);
                        bw.Write(num.Length);
                        bw.Write(num);

                        num = BigInteger.Abs(r.Denominator).ToByteArray();
                        bw.Write(num.Length);
                        bw.Write(num);
                    }
                    bw.Write(opt.IterationCount);
                    bw.Write(opt.Width);
                    bw.Write(opt.Height);
                    bw.Write(opt.XOff);
                    bw.Write(opt.XSkip);
                    bw.Write(opt.XMax);
                    bw.Write(opt.YOff);
                    bw.Write(opt.YSkip);
                    bw.Write(opt.YMax);
                    bw.Write(opt.OrbitStart);
                    bw.Write(opt.OrbitLength);
                    bw.Write(opt.BulbChecking ? 1U : 0U);

                    int ok = br.ReadInt32();
                    if (ok != 200)
                        throw new InvalidOperationException();  // TODO Proper exception

                    // Sent parameters, now read all orbits
                    OrbitMap map = new OrbitMap();                    

                    int orbitsComing = 0;
                    while ((orbitsComing = br.ReadInt32()) >= 0)
                    {
                        for (int i = 0; i < orbitsComing; i++)
                        {
                            int o_x = br.ReadInt32();
                            int o_y = br.ReadInt32();

                            BigRational re, im;
                            re = ReadRational(br);
                            im = ReadRational(br);
                            BigComplex o_c = new BigComplex(re, im);

                            uint o_iter = br.ReadUInt32(); // Number of iterations before escape
                            uint o_orbs = br.ReadUInt32(); // Offset of the orbit
                            uint o_orbl = br.ReadUInt32(); // Length of the orbit

                            Orbit orbit = new Orbit(o_x, o_y, o_c, o_iter, o_orbs, o_orbl);

                            for (uint j = 0; j < o_orbl; j++)
                            {
                                re = ReadRational(br);
                                im = ReadRational(br);
                                orbit.Add(re, im);
                            }

                            map.AddOrbit(orbit);
                        }
                    }

                    Result = map; // Set the result so it can be combined with work from the other threads
                }

                ns.Close();
                Success = true;
            }
            catch (Exception) // TODO error handling ? Just log the message and be done
            {
                Success = false;
            }
            finally
            {
                if (ns != null)
                    ns.Close();
                
                (WaitHandle as ManualResetEvent).Set(); // Public signal that I'm done
            }
        }

        internal BigRational ReadRational(EndianBinaryReader reader)
        {
            int sign = reader.ReadInt32();
            long len = reader.ReadInt64();
            byte[] num = new byte[len + 1]; // Extra byte for sign forcing positive value
            long l = ReadLongBuffer(num, len, reader);

            if (l != len)
                throw new InvalidOperationException(); // Non recoverable.. 

            len = reader.ReadInt64();
            byte[] den = new byte[len + 1]; // Extra byte on the end for positive sign
            l = ReadLongBuffer(den, len, reader);

            if (l != len)
                throw new InvalidOperationException(); // TODO Create a new ProtocolException() for when server sends unexpected data

            return new BigRational(new BigInteger(num) * sign, new BigInteger(den));
        }

        internal long ReadLongBuffer(byte[] buffer, long len, EndianBinaryReader reader)
        {
            // TODO check whole function

            byte[] temp = (len > int.MaxValue ? new byte[int.MaxValue] : null);

            long byt = 0;
            while (byt < len)
            {
                int n;
                long missing = len - byt;
                int missing32 = (missing > int.MaxValue ? int.MaxValue : (int)missing);
                if (temp != null) // Use a temporary buffer so offsets fit in an int
                {
                    n = reader.Read(temp, 0, missing32);
                    Array.Copy(temp, 0, buffer, byt, n);
                }
                else // Total package is shorter than int.MaxValue
                {
                    n = reader.Read(buffer, (int)byt, (int)missing);
                }
                byt += n;
            }

            return byt;
        }
    }
}