using System.Text.Json;
using karst.Models;

namespace karst.Services;

public class JsonDataService : IDataService
{
    private readonly string _basePath;
    private readonly JsonSerializerOptions _jsonOptions;

    public JsonDataService()
    {
        _basePath = FileSystem.AppDataDirectory;
        _jsonOptions = new JsonSerializerOptions
        {
            WriteIndented = true,
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase
        };
    }

    public async Task<List<Folder>> GetFoldersAsync()
    {
        var foldersPath = Path.Combine(_basePath, "folders");
        Directory.CreateDirectory(foldersPath);

        var folders = new List<Folder>();
        foreach (var file in Directory.GetFiles(foldersPath, "*.json"))
        {
            try
            {
                var json = await File.ReadAllTextAsync(file);
                var folder = JsonSerializer.Deserialize<Folder>(json, _jsonOptions);
                if (folder != null)
                    folders.Add(folder);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"Error loading folder {file}: {ex.Message}");
            }
        }

        return folders;
    }

    public async Task<Folder> GetFolderAsync(string id)
    {
        var filePath = Path.Combine(_basePath, "folders", $"{id}.json");
        if (!File.Exists(filePath))
            return null;

        try
        {
            var json = await File.ReadAllTextAsync(filePath);
            return JsonSerializer.Deserialize<Folder>(json, _jsonOptions);
        }
        catch
        {
            return null;
        }
    }

    public async Task SaveFolderAsync(Folder folder)
    {
        var foldersPath = Path.Combine(_basePath, "folders");
        Directory.CreateDirectory(foldersPath);

        var filePath = Path.Combine(foldersPath, $"{folder.Id}.json");
        var json = JsonSerializer.Serialize(folder, _jsonOptions);
        await File.WriteAllTextAsync(filePath, json);
    }

    public async Task DeleteFolderAsync(string id)
    {
        var filePath = Path.Combine(_basePath, "folders", $"{id}.json");
        if (File.Exists(filePath))
        {
            File.Delete(filePath);
        }

        // Also delete all notes in this folder
        var notes = await GetNotesInFolderAsync(id);
        foreach (var note in notes)
        {
            await DeleteNoteAsync(note.Id);
        }
    }

    public async Task<Note> GetNoteAsync(string id)
    {
        var filePath = Path.Combine(_basePath, "notes", $"{id}.json");
        if (!File.Exists(filePath))
            return null;

        try
        {
            var json = await File.ReadAllTextAsync(filePath);
            return JsonSerializer.Deserialize<Note>(json, _jsonOptions);
        }
        catch
        {
            return null;
        }
    }

    public async Task SaveNoteAsync(Note note)
    {
        var notesPath = Path.Combine(_basePath, "notes");
        Directory.CreateDirectory(notesPath);

        var filePath = Path.Combine(notesPath, $"{note.Id}.json");
        var json = JsonSerializer.Serialize(note, _jsonOptions);
        await File.WriteAllTextAsync(filePath, json);
    }

    public async Task DeleteNoteAsync(string id)
    {
        var filePath = Path.Combine(_basePath, "notes", $"{id}.json");
        if (File.Exists(filePath))
        {
            File.Delete(filePath);
        }
    }

    public async Task<List<Note>> GetNotesInFolderAsync(string folderId)
    {
        var notesPath = Path.Combine(_basePath, "notes");
        if (!Directory.Exists(notesPath))
            return new List<Note>();

        var notes = new List<Note>();
        foreach (var file in Directory.GetFiles(notesPath, "*.json"))
        {
            try
            {
                var json = await File.ReadAllTextAsync(file);
                var note = JsonSerializer.Deserialize<Note>(json, _jsonOptions);
                if (note != null && note.FolderId == folderId)
                    notes.Add(note);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"Error loading note {file}: {ex.Message}");
            }
        }

        return notes;
    }
}