using CommunityToolkit.Mvvm.ComponentModel;

namespace karst.Models;

public partial class Folder : ObservableObject
{
    [ObservableProperty]
    private string id;

    [ObservableProperty]
    private string name;

    [ObservableProperty]
    private string parentId;

    [ObservableProperty]
    private DateTime createdDate;

    [ObservableProperty]
    private List<Note> notes;

    public Folder()
    {
        Id = Guid.NewGuid().ToString();
        CreatedDate = DateTime.Now;
        Notes = new List<Note>();
    }
}