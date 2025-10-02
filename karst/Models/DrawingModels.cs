using CommunityToolkit.Mvvm.ComponentModel;
using SkiaSharp;

namespace karst.Models;

public partial class Stroke : ObservableObject
{
    [ObservableProperty]
    private List<SKPoint> points;

    [ObservableProperty]
    private SKColor color;

    [ObservableProperty]
    private float size;

    [ObservableProperty]
    private ToolType toolType;

    public Stroke()
    {
        Points = new List<SKPoint>();
        Color = SKColors.Black;
        Size = 2f;
        ToolType = ToolType.Pen;
    }
}

public partial class ImageAnnotation : ObservableObject
{
    [ObservableProperty]
    private string id;

    [ObservableProperty]
    private string imagePath;

    [ObservableProperty]
    private SKRect bounds;

    [ObservableProperty]
    private float rotation;

    public ImageAnnotation()
    {
        Id = Guid.NewGuid().ToString();
        Bounds = new SKRect(100, 100, 300, 300);
    }
}

public enum ToolType
{
    Pen,
    Highlighter,
    Eraser
}

public class DrawingTool
{
    public ToolType Type { get; set; }
    public string Name { get; set; }
    public SKColor Color { get; set; }
    public float Size { get; set; }
    public string Icon { get; set; }
}