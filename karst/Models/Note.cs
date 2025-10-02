using System.ComponentModel;
using CommunityToolkit.Mvvm.ComponentModel;

namespace karst.Models;

public partial class Note : ObservableObject
{
    [ObservableProperty]
    private string id;

    [ObservableProperty]
    private string title;

    [ObservableProperty]
    private string content;

    [ObservableProperty]
    private string folderId;

    [ObservableProperty]
    private DateTime createdDate;

    [ObservableProperty]
    private DateTime modifiedDate;

    [ObservableProperty]
    private PaperType paperType;

    [ObservableProperty]
    private PaperSize paperSize;

    [ObservableProperty]
    private List<Stroke> strokes;

    [ObservableProperty]
    private List<ImageAnnotation> images;

    public Note()
    {
        Id = Guid.NewGuid().ToString();
        CreatedDate = DateTime.Now;
        ModifiedDate = DateTime.Now;
        Title = "New Note";
        Strokes = new List<Stroke>();
        Images = new List<ImageAnnotation>();
    }
}

public enum PaperType
{
    Plain,
    Ruled
}

public enum PaperSize
{
    A3,
    A4
}