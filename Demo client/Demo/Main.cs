using FractalRenderer;
using FractalRenderer.Colorizers;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Net;
using System.Windows.Forms;

namespace FractalClient
{
    public partial class Main : Form
    {
        int ci = 0;

        private ExternalMandelbrotCalculator calc;
        private List<Colorizer> colorizers;

        public Main(string[] args)
        {
            InitializeComponent();

            IPEndPoint ep = null;
            if (args.Length > 0)
            {
                string[] split = args[0].Split(':');
                if (split.Length == 2)
                {
                    IPAddress ip;
                    int port;
                    if (!IPAddress.TryParse(split[0], out ip))
                    {
                        IPAddress[] addr = Dns.GetHostAddresses(split[0]);
                        if (addr.Length == 0)
                            Console.WriteLine("Invalid IP address");
                        else
                            ip = addr[0];
                    }
                    if (ip != null)
                    {
                        if (!int.TryParse(split[1], out port))
                            Console.WriteLine("Invalid port");
                        else
                            ep = new IPEndPoint(ip, port);
                    }
                }
                else
                {
                    Console.WriteLine("Invalid arguments");
                }
            }

            RenderHost host;
            if (ep != null)
            {
                host = new RenderHost(ep, 8);
            }
            else
            {
                host = new RenderHost(new IPEndPoint(Dns.GetHostAddresses("samplehost.net")[0], 443),
                                      8); // Address of external render host, and number of tasks this host can perform simultaneously
            }


            calc = new ExternalMandelbrotCalculator();
            calc.HostSelector = new HostSelector();
            calc.HostSelector.AddHost(host);

            colorizers = new List<Colorizer>();
            colorizers.Add(new MandelbrotColorizer());
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Colorizer c = colorizers[ci++ % colorizers.Count];

            CalculationOptions opt = new CalculationOptions(pictureBox1.Width, pictureBox1.Height, 100);
            opt.BulbChecking = true;
            opt.OrbitLength = 0;

            RenderOptions rp = new RenderOptions() { CanvasWidth = pictureBox1.Width, CanvasHeight = pictureBox1.Height };

            OrbitMap m = calc.Calculate(opt);
            Bitmap b = c.DrawBitmap(m, rp, opt, null);

            pictureBox1.Image = b;
        }
    }
}
