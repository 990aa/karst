using SkiaSharp;
using SkiaSharp.Views.Maui;

namespace karst.Services
{
    public class SkiaPdfExportService : IPdfExportService
    {
        public Task<string> ExportToPdfAsync(karst.Models.Note note)
        {
            // Dummy implementation for build
            return Task.FromResult(string.Empty);
        }
    }
}
