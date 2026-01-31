#pragma once
#include <string>
#include <__filesystem/filesystem_error.h>

namespace SSGEEditor
{
    class CSharpCompiler
    {
    public:
        CSharpCompiler() = delete;
        [[nodiscard]] static auto compile(const std::filesystem::path& projectPath,
                                          const std::string& projectName) -> std::string;

    };
} // SSGEEditor
