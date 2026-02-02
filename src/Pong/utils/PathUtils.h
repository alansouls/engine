#pragma once

#include <filesystem>
#include <string>

namespace SSGE
{
namespace Utils
{

/// @brief Get the directory containing the current executable
/// @return Path to the directory containing the executable
std::filesystem::path getExecutableDirectory();

/// @brief Calculate the .NET project path relative to the executable location
/// @details Assumes executable is in src/cmake_build/bin/ (or similar)
/// @return Normalized path to the .NET project directory (../../dotnet/SSGEDotNet from executable)
std::string getDebugDotnetPath();

} // namespace Utils
} // namespace SSGE
