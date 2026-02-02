#include "SSGEEditor.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#include <unistd.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#ifdef NDEBUG
constexpr bool debugModeOn = false;
#else
constexpr bool debugModeOn = true;
#endif

// Helper function to get the executable directory
std::filesystem::path getExecutableDirectory()
{
    // Get the path of the current executable
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
#elif __APPLE__
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0)
    {
        // Resolve any symlinks to get the real path
        char realPath[PATH_MAX];
        if (realpath(buffer, realPath) != nullptr)
        {
            return std::filesystem::path(realPath).parent_path();
        }
        return std::filesystem::path(buffer).parent_path();
    }
    // Fallback to current directory
    return std::filesystem::current_path();
#else
    // Linux and other Unix-like systems
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1)
    {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).parent_path();
    }
    // Fallback to current directory
    return std::filesystem::current_path();
#endif
}

// Helper function to calculate relative dotnet path from executable
std::string getDebugDotnetPath()
{
    // Executable is expected to be in src/cmake_build/bin/ (or similar)
    // So ../../ from executable location should be in src/
    auto execDir = getExecutableDirectory();
    auto dotnetPath = execDir / ".." / ".." / "dotnet" / "SSGEDotNet";
    return dotnetPath.lexically_normal().string();
}

void printUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " [options]\n"
              << "Options:\n"
              << "  --dotnet-project-path <path>  Specify the path to the .NET project directory\n"
              << "  --debug-paths                 Use relative paths based on executable location\n"
              << "  --help                        Display this help message\n";
}

int main(int argc, char *argv[])
{
    std::string dotnetProjectPath;
    bool useDebugPaths = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--dotnet-project-path")
        {
            if (i + 1 < argc)
            {
                dotnetProjectPath = argv[++i];
            }
            else
            {
                std::cerr << "Error: --dotnet-project-path requires a path argument\n";
                printUsage(argv[0]);
                return EXIT_FAILURE;
            }
        }
        else if (arg == "--debug-paths")
        {
            useDebugPaths = true;
        }
        else if (arg == "--help")
        {
            printUsage(argv[0]);
            return EXIT_SUCCESS;
        }
        else
        {
            std::cerr << "Error: Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Determine the dotnet project path
    if (useDebugPaths)
    {
        dotnetProjectPath = getDebugDotnetPath();
        std::cout << "Using debug paths. DotNet project path: " << dotnetProjectPath << std::endl;
    }
    else if (dotnetProjectPath.empty())
    {
        std::cerr << "Error: No .NET project path specified. Use --dotnet-project-path <path> or --debug-paths\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // Verify that the dotnet project path exists
    // Note: We only warn here and don't fail, allowing the application to start
    // even if the path doesn't exist. This is useful for development scenarios.
    if (!std::filesystem::exists(dotnetProjectPath))
    {
        std::cerr << "Warning: DotNet project path does not exist: " << dotnetProjectPath << std::endl;
        std::cerr << "The application will continue, but C# scripting features may not work correctly." << std::endl;
    }

    try
    {
        SSGEEditor app;
        app.run(debugModeOn, dotnetProjectPath);
    }
    catch (const std::exception &e)
    {
        std::printf("%s\n", e.what());
        throw;
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
