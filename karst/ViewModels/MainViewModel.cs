using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using karst.Models;
using karst.Services;

namespace karst.ViewModels;

public partial class MainViewModel : ObservableObject
{
    private readonly IDataService _dataService;

    [ObservableProperty]
    private List<Folder> folders;

    [ObservableProperty]
    private Folder selectedFolder;

    [ObservableProperty]
    private Note selectedNote;

    [ObservableProperty]
    private bool isEditing;

    public MainViewModel(IDataService dataService)
    {
        _dataService = dataService;
        Folders = new List<Folder>();
        InitializeAsync();
    }

    private async void InitializeAsync()
    {
        await LoadFolders();
    }

    [RelayCommand]
    private async Task LoadFolders()
    {
        try
        {
            Folders = await _dataService.GetFoldersAsync();
        }
        catch (Exception ex)
        {
            await Shell.Current.DisplayAlert("Error", $"Failed to load folders: {ex.Message}", "OK");
        }
    }

    [RelayCommand]
    private async Task CreateFolder()
    {
        try
        {
            var name = await Shell.Current.DisplayPromptAsync("New Folder", "Enter folder name:");
            if (!string.IsNullOrWhiteSpace(name))
            {
                var folder = new Folder { Name = name.Trim() };
                await _dataService.SaveFolderAsync(folder);
                await LoadFolders();
            }
        }
        catch (Exception ex)
        {
            await Shell.Current.DisplayAlert("Error", $"Failed to create folder: {ex.Message}", "OK");
        }
    }

    [RelayCommand]
    private async Task DeleteFolder(Folder folder)
    {
        try
        {
            bool confirm = await Shell.Current.DisplayAlert("Delete Folder", 
                $"Are you sure you want to delete '{folder.Name}' and all its contents?", "Yes", "No");
            
            if (confirm)
            {
                await _dataService.DeleteFolderAsync(folder.Id);
                await LoadFolders();
            }
        }
        catch (Exception ex)
        {
            await Shell.Current.DisplayAlert("Error", $"Failed to delete folder: {ex.Message}", "OK");
        }
    }

    [RelayCommand]
    private async Task CreateNote()
    {
        if (SelectedFolder == null)
        {
            await Shell.Current.DisplayAlert("Error", "Please select a folder first", "OK");
            return;
        }

        try
        {
            await Shell.Current.GoToAsync($"//NoteEditor?folderId={SelectedFolder.Id}&noteId=new");
        }
        catch (Exception ex)
        {
            await Shell.Current.DisplayAlert("Error", $"Failed to create note: {ex.Message}", "OK");
        }
    }

    [RelayCommand]
    private async Task OpenNote(Note note)
    {
        try
        {
            await Shell.Current.GoToAsync($"//NoteEditor?folderId={note.FolderId}&noteId={note.Id}");
        }
        catch (Exception ex)
        {
            await Shell.Current.DisplayAlert("Error", $"Failed to open note: {ex.Message}", "OK");
        }
    }
}