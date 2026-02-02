#pragma once

#include <string>

namespace SSGE
{
namespace Utils
{

/// @brief Parsed command line arguments for the engine
struct ArgumentParserResult
{
    std::string dotnetProjectPath;
    bool useDebugPaths = false;
    bool shouldExit = false;
    int exitCode = 0;
};

/// @brief Parse command line arguments for the engine editor
class ArgumentParser
{
  public:
    /// @brief Parse command line arguments
    /// @param argc Argument count
    /// @param argv Argument values
    /// @return Parsed arguments or exit information
    static ArgumentParserResult parse(int argc, char *argv[]);

  private:
    /// @brief Print usage information
    /// @param programName Name of the program
    static void printUsage(const char *programName);
};

} // namespace Utils
} // namespace SSGE
