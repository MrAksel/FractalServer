namespace FractalRenderer.Colorizers
{
    public struct RenderOptions
    {
        public static readonly RenderOptions Empty = default(RenderOptions);

        public int CanvasWidth
        {
            get;
            set;
        }

        public int CanvasHeight
        {
            get;
            set;
        }


    }
}