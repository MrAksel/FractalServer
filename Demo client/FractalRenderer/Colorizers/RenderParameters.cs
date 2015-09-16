namespace FractalRenderer.Colorizers
{
    public struct RenderParameters
    {
        public static readonly RenderParameters Empty = default(RenderParameters);

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