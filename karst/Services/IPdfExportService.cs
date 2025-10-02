using karst.Models;

namespace karst.Services;

public interface IPdfExportService
{
    Task<string> ExportToPdfAsync(Note note);
}