using karst.Models;

namespace karst.Services;

public interface IDataService
{
    Task<List<Folder>> GetFoldersAsync();
    Task<Folder> GetFolderAsync(string id);
    Task SaveFolderAsync(Folder folder);
    Task DeleteFolderAsync(string id);
    
    Task<Note> GetNoteAsync(string id);
    Task SaveNoteAsync(Note note);
    Task DeleteNoteAsync(string id);
    Task<List<Note>> GetNotesInFolderAsync(string folderId);
}