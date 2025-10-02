using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using karst.Models;
using karst.Services;
using SkiaSharp;

namespace karst.ViewModels;

public partial class NoteEditorViewModel : ObservableObject, IQueryAttributable
{
    public List<PaperType> PaperTypes { get; } = Enum.GetValues(typeof(PaperType)).Cast<PaperType>().ToList();
    public List<PaperSize> PaperSizes { get; } = Enum.GetValues(typeof(PaperSize)).Cast<PaperSize>().ToList();
    private readonly IDataService _dataService;
    private readonly IPdfExportService _pdfExportService;


    [ObservableProperty]
    private Note? currentNote;

    [ObservableProperty]
    private Folder? currentFolder;

    [ObservableProperty]
    private DrawingTool? selectedTool;

    [ObservableProperty]
    private List<DrawingTool>? availableTools;

    [ObservableProperty]
    private bool isDrawingEnabled = true;

    [ObservableProperty]
    private SKColor currentColor = SKColors.Black;

    [ObservableProperty]
    private float currentSize = 2f;

    [ObservableProperty]
    private PaperType selectedPaperType = PaperType.Plain;

    [ObservableProperty]
    private PaperSize selectedPaperSize = PaperSize.A4;

    [ObservableProperty]
    private List<SKColor>? availableColors;

    private Stroke? currentStroke;

    public NoteEditorViewModel(IDataService dataService, IPdfExportService pdfExportService)
    {
        _dataService = dataService;
        _pdfExportService = pdfExportService;
        InitializeTools();
    }

    private void InitializeTools()
    {
        AvailableTools = new List<DrawingTool>
        {
            new() { Type = ToolType.Pen, Name = "Pen", Color = SKColors.Black, Size = 2f, Icon = "✏️" },
            new() { Type = ToolType.Highlighter, Name = "Highlighter", Color = SKColors.Yellow.WithAlpha(128), Size = 8f, Icon = "🖍️" },
            new() { Type = ToolType.Eraser, Name = "Eraser", Color = SKColors.Transparent, Size = 20f, Icon = "🧹" }
        };

        SelectedTool = AvailableTools[0];

        AvailableColors = new List<SKColor>
        {
            SKColors.Black,
            SKColors.Red,
            SKColors.Blue,
            SKColors.Green,
            SKColors.Purple,
            SKColors.Orange,
            SKColors.Yellow,
            SKColors.Pink
        };
    }

    [RelayCommand]
    private void StartStroke(SKPoint point)
    {
        if (!IsDrawingEnabled) return;

        currentStroke = new Stroke
        {
            Color = SelectedTool.Type == ToolType.Eraser ? SKColors.Transparent : CurrentColor,
            Size = CurrentSize,
            ToolType = SelectedTool.Type
        };
        currentStroke.Points.Add(point);
    }

    [RelayCommand]
    private void AddPointToStroke(SKPoint point)
    {
        if (currentStroke == null || !IsDrawingEnabled) return;
        currentStroke.Points.Add(point);
    }

    [RelayCommand]
    private void EndStroke()
    {
        if (currentStroke != null && currentStroke.Points.Count > 0 && CurrentNote != null)
        {
            CurrentNote.Strokes.Add(currentStroke);
            currentStroke = null;
            SaveNoteCommand.Execute(null);
        }
    }

    [RelayCommand]
    private void ClearCanvas()
    {
        CurrentNote.Strokes.Clear();
        SaveNoteCommand.Execute(null);
    }

    [RelayCommand]
    private async Task SaveNote()
    {
        try
        {
            CurrentNote.ModifiedDate = DateTime.Now;
            await _dataService.SaveNoteAsync(CurrentNote);
        }
        catch (Exception ex)
        {
            await Shell.Current.DisplayAlert("Error", $"Failed to save note: {ex.Message}", "OK");
        }
    }

    [RelayCommand]
    private async Task ExportToPdf()
    {
        try
        {
            var filePath = await _pdfExportService.ExportToPdfAsync(CurrentNote);
            await Shell.Current.DisplayAlert("Success", $"Note exported to: {filePath}", "OK");
        }
        catch (Exception ex)
        {
            await Shell.Current.DisplayAlert("Error", $"Failed to export PDF: {ex.Message}", "OK");
        }
    }

    [RelayCommand]
    private async Task PickImage()
    {
        try
        {
            var result = await FilePicker.PickAsync(new PickOptions
            {
                PickerTitle = "Pick an image",
                FileTypes = FilePickerFileType.Images
            });

            if (result != null)
            {
                var imageAnnotation = new ImageAnnotation
                {
                    ImagePath = result.FullPath
                };
                CurrentNote.Images.Add(imageAnnotation);
                await SaveNote();
            }
        }
        catch (Exception ex)
        {
            await Shell.Current.DisplayAlert("Error", $"Failed to pick image: {ex.Message}", "OK");
        }
    }

    [RelayCommand]
    private async Task GoBack()
    {
        await SaveNote();
        await Shell.Current.GoToAsync("//MainPage");
    }

    public async void ApplyQueryAttributes(IDictionary<string, object> query)
    {
        if (query.TryGetValue("folderId", out object folderIdObj) &&
            query.TryGetValue("noteId", out object noteIdObj))
        {
            var folderId = folderIdObj as string;
            var noteId = noteIdObj as string;

            await LoadNote(folderId, noteId);
        }
    }

    private async Task LoadNote(string folderId, string noteId)
    {
        try
        {
            CurrentFolder = await _dataService.GetFolderAsync(folderId);

            if (noteId == "new")
            {
                CurrentNote = new Note
                {
                    FolderId = folderId,
                    Title = $"Note {DateTime.Now:yyyy-MM-dd HHmmss}",
                    PaperType = SelectedPaperType,
                    PaperSize = SelectedPaperSize
                };
            }
            else
            {
                CurrentNote = await _dataService.GetNoteAsync(noteId);
                SelectedPaperType = CurrentNote.PaperType;
                SelectedPaperSize = CurrentNote.PaperSize;
            }
        }
        catch (Exception ex)
        {
            await Shell.Current.DisplayAlert("Error", $"Failed to load note: {ex.Message}", "OK");
        }
    }

    partial void OnSelectedToolChanged(DrawingTool value)
    {
        if (value != null)
        {
            CurrentColor = value.Color;
            CurrentSize = value.Size;
        }
    }

    partial void OnSelectedPaperTypeChanged(PaperType value)
    {
        if (CurrentNote != null)
        {
            CurrentNote.PaperType = value;
            SaveNoteCommand.Execute(null);
        }
    }

    partial void OnSelectedPaperSizeChanged(PaperSize value)
    {
        if (CurrentNote != null)
        {
            CurrentNote.PaperSize = value;
            SaveNoteCommand.Execute(null);
        }
    }
}