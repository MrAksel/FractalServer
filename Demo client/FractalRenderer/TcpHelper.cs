using System;
using System.Net;
using System.Net.Sockets;

namespace FractalRenderer
{
    internal class TcpHelper
    {
        internal static NetworkStream OpenStream(IPEndPoint host, int maxtries = 3)
        {
            int tries = 0;
            while (true)
            {
                try
                {
                    Socket s = new Socket(SocketType.Stream, ProtocolType.Tcp);
                    s.Connect(host);
                    return new NetworkStream(s, true);
                }
                catch (Exception) // TODO Specific exceptions, SocketException ?
                {
                    // TODO Log error
                    tries++;
                    if (tries >= maxtries)
                    {
                        throw;
                    }
                }
            }
        }
    }
}