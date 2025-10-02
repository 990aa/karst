using karst.ViewModels;
using SkiaSharp;
using SkiaSharp.Views.Maui;

namespace karst.Views;

public partial class NoteEditorPage : ContentPage
{
    private NoteEditorViewModel ViewModel => BindingContext as NoteEditorViewModel;

    public NoteEditorPage(NoteEditorViewModel viewModel)
    {
        InitializeComponent();
        BindingContext = viewModel;
    }

    private void OnCanvasPaintSurface(object sender, SKPaintSurfaceEventArgs e)
    {
        var canvas = e.Surface.Canvas;
        canvas.Clear(SKColors.White);

        if (ViewModel?.CurrentNote == null) return;

        DrawPaperPattern(canvas, e.Info);
        DrawStrokes(canvas);
        DrawImages(canvas);
    }

    private void DrawPaperPattern(SKCanvas canvas, SKImageInfo info)
    {
        if (ViewModel.SelectedPaperType == Models.PaperType.Ruled)
        {
            using var paint = new SKPaint
            {
                Color = SKColors.LightGray.WithAlpha(64),
                StrokeWidth = 1,
                IsAntialias = true
            };

            float lineSpacing = 20;
            for (float y = 50; y < info.Height; y += lineSpacing)
            {
                canvas.DrawLine(0, y, info.Width, y, paint);
            }
        }
    }

    private void DrawStrokes(SKCanvas canvas)
    {
        foreach (var stroke in ViewModel.CurrentNote.Strokes)
        {
            DrawStroke(canvas, stroke);
        }
    }

    private void DrawStroke(SKCanvas canvas, Models.Stroke stroke)
    {
        if (stroke.Points.Count < 2) return;

        using var paint = new SKPaint
        {
            Color = stroke.Color,
            StrokeWidth = stroke.Size,
            StrokeCap = SKStrokeCap.Round,
            StrokeJoin = SKStrokeJoin.Round,
            Style = SKPaintStyle.Stroke,
            IsAntialias = true
        };

        if (stroke.ToolType == Models.ToolType.Highlighter)
        {
            paint.Color = paint.Color.WithAlpha(128);
            paint.BlendMode = SKBlendMode.Multiply;
        }

        using var path = new SKPath();
        path.MoveTo(stroke.Points[0]);

        for (int i = 1; i < stroke.Points.Count; i++)
        {
            path.LineTo(stroke.Points[i]);
        }

        canvas.DrawPath(path, paint);
    }

    private void DrawImages(SKCanvas canvas)
    {
        foreach (var image in ViewModel.CurrentNote.Images)
        {
            DrawImage(canvas, image);
        }
    }

    private void DrawImage(SKCanvas canvas, Models.ImageAnnotation image)
    {
        if (!File.Exists(image.ImagePath)) return;

        try
        {
            using var skImage = SKImage.FromEncodedData(image.ImagePath);
            if (skImage != null)
            {
                var destRect = image.Bounds;
                canvas.DrawImage(skImage, destRect);
            }
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Error drawing image: {ex.Message}");
        }
    }

    private void OnCanvasTouch(object sender, SKTouchEventArgs e)
    {
        if (!ViewModel.IsDrawingEnabled) return;

        switch (e.ActionType)
        {
            case SKTouchAction.Pressed:
                ViewModel.StartStrokeCommand.Execute(e.Location);
                e.Handled = true;
                break;

            case SKTouchAction.Moved:
                ViewModel.AddPointToStrokeCommand.Execute(e.Location);
                e.Handled = true;
                break;

            case SKTouchAction.Released:
            case SKTouchAction.Cancelled:
                ViewModel.EndStrokeCommand.Execute(null);
                e.Handled = true;
                break;
        }

        // Force redraw
        ((SKCanvasView)sender).InvalidateSurface();
    }

    protected override void OnAppearing()
    {
        base.OnAppearing();
        if (BindingContext is NoteEditorViewModel vm)
        {
            // Refresh canvas
            var canvas = this.FindByName<SKCanvasView>("DrawingCanvas");
            canvas?.InvalidateSurface();
        }
    }
}