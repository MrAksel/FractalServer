using FractalRenderer;
using FractalRenderer.Colorizers;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace FractalClient
{
    public partial class Main : Form
    {
        int ci = 0;

        private ExternalMandelbrotCalculator calc;
        private List<Colorizer> colorizers;

        public Main()
        {
            InitializeComponent();

            //RenderHost host = new RenderHost(new System.Net.IPEndPoint(System.Net.Dns.GetHostAddresses("sampleaddress.domain.tl")[0], 40876),
            //                                 8); // Address of external render host, and number of tasks this host can perform simultaneously

            //RenderHost host = new RenderHost(new System.Net.IPEndPoint(System.Net.IPAddress.Loopback, 50871), 8);

            RenderHost host = new RenderHost(new System.Net.IPEndPoint(System.Net.IPAddress.Parse("192.168.1.150"), 3987),
                                               1);

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
            opt.BulbChecking = false;

            RenderOptions rp = new RenderOptions() { CanvasWidth = pictureBox1.Width, CanvasHeight = pictureBox1.Height };

            OrbitMap m = calc.Calculate(opt);
            Bitmap b = c.DrawBitmap(m, rp, null);

            pictureBox1.Image = b;
        }
    }
}
