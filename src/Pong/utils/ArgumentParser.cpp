#include "ArgumentParser.h"
#include "PathUtils.h"
#include <filesystem>
#include <iostream>

namespace SSGE
{
namespace Utils
{

void ArgumentParser::printUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " [options]\n"
              << "Options:\n"
              << "  --dotnet-project-path <path>  Specify the path to the .NET project directory\n"
              << "  --debug-paths                 Use relative paths based on executable location\n"
              << "  --help                        Display this help message\n";
}

ArgumentParserResult ArgumentParser::parse(int argc, char *argv[])
{
    ArgumentParserResult result;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--dotnet-project-path")
        {
            if (i + 1 < argc)
            {
                result.dotnetProjectPath = argv[++i];
            }
            else
            {
                std::cerr << "Error: --dotnet-project-path requires a path argument\n";
                printUsage(argv[0]);
                result.shouldExit = true;
                result.exitCode = EXIT_FAILURE;
                return result;
            }
        }
        else if (arg == "--debug-paths")
        {
            result.useDebugPaths = true;
        }
        else if (arg == "--help")
        {
            printUsage(argv[0]);
            result.shouldExit = true;
            result.exitCode = EXIT_SUCCESS;
            return result;
        }
        else
        {
            std::cerr << "Error: Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            result.shouldExit = true;
            result.exitCode = EXIT_FAILURE;
            return result;
        }
    }

    // Determine the dotnet project path
    // Note: --dotnet-project-path takes precedence over --debug-paths
    if (result.useDebugPaths && result.dotnetProjectPath.empty())
    {
        result.dotnetProjectPath = getDebugDotnetPath();
        std::cout << "Using debug paths. .NET project path: " << result.dotnetProjectPath << std::endl;
    }
    else if (result.dotnetProjectPath.empty())
    {
        // No project path provided — the startup popup will let the user open or create one
    }

    // Verify that the dotnet project path exists
    // Note: We only warn here and don't fail, allowing the application to start
    // even if the path doesn't exist. This is useful for development scenarios.
    if (!std::filesystem::exists(result.dotnetProjectPath))
    {
        std::cerr << "Warning: .NET project path does not exist: " << result.dotnetProjectPath << std::endl;
        std::cerr << "The application will continue, but C# scripting features may not work correctly." << std::endl;
    }

    return result;
}

} // namespace Utils
} // namespace SSGE
