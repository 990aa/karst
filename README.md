# Karst

A cross-platform drawing and note-taking application using .NET MAUI and a native C++ drawing library.

## Project Structure

- `karst/` - Main .NET MAUI app (C#)
- `NativeLibrary/` - Native C++ drawing library

## Building the Project

### Prerequisites
- .NET 8 SDK (for MAUI)
- Visual Studio 2022 or later (with MAUI and Desktop/Android workloads)
- C++ build tools (for NativeLibrary)

### Build Steps
1. **Restore NuGet packages**
   ```pwsh
   dotnet restore karst/karst.csproj
   ```
2. **Build the native library**
   - Open `NativeLibrary/NativeLibrary.vcxproj` in Visual Studio and build for your target platform (x64/Win32/Android).

3. **Build the MAUI app**
   ```pwsh
   dotnet build karst/karst.csproj
   ```

4. **Run the app**
   ```pwsh
   dotnet run --project karst/karst.csproj
   ```

## Notes
- The native library must be built for each platform you wish to target.
- For Android, ensure the native library is copied to the correct ABI folder.
- For Windows, the DLL must be accessible to the MAUI app (e.g., in output directory).

## License
MIT
