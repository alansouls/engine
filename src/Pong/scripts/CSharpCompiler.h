#pragma once
#include <string>
#include <filesystem>

namespace SSGE
{
    class CSharpCompiler
    {
    public:
        CSharpCompiler() = delete;
        [[nodiscard]] static auto compile(const std::filesystem::path& projectPath,
                                          const std::string& projectName) -> std::string;

    };
} // SSGE
