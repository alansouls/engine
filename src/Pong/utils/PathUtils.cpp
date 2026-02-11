#include "PathUtils.h"

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <limits.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

namespace SSGE
{
namespace Utils
{

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
    char buffer[PATH_MAX] = {0}; // Initialize buffer
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

std::string getDebugDotnetPath()
{
    // Executable is expected to be in src/cmake_build/bin/ (or similar)
    // So ../../ from executable location should be in src/
    auto execDir = getExecutableDirectory();
    auto dotnetPath = execDir / ".." / ".." / "dotnet" / "SSGEDotNet";
    return dotnetPath.lexically_normal().string();
}

} // namespace Utils
} // namespace SSGE
