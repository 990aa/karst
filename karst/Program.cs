namespace karst;

public class Program
{
    public static void Main(string[] args)
    {
        var builder = MauiApp.CreateBuilder();
        var mauiApp = MauiProgram.CreateMauiApp();
        var serviceProvider = mauiApp.Services;
        var app = new App(serviceProvider);
        // If needed, add code to run the app for your platform
    }
}