#pragma once
#include <filesystem>
#include <string>

namespace SSGE
{
class CSharpCompiler
{
  public:
    CSharpCompiler() = delete;
    [[nodiscard]] static auto compile(const std::filesystem::path &projectPath, const std::string &projectName)
        -> std::string;
};
} // namespace SSGE
