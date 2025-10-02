using karst.Services;
using karst.ViewModels;
using karst.Views;

namespace karst;

public partial class App : Application
{
    public App(IServiceProvider serviceProvider)
    {
        InitializeComponent();

        MainPage = new AppShell(serviceProvider);
    }
}

public class AppShell : Shell
{
    public AppShell(IServiceProvider serviceProvider)
    {
        // Register routes
        Routing.RegisterRoute("//MainPage", typeof(MainPage));
        Routing.RegisterRoute("//NoteEditor", typeof(NoteEditorPage));

        // Create main page
        Items.Add(new ShellContent
        {
            Title = "Notes",
            Route = "MainPage",
            Content = serviceProvider.GetService<MainPage>()
        });
    }
}