//
// Created by Alan Maia on 31/01/2026.
//

#include "CSharpCompiler.h"

#include <format>
#include <filesystem>

namespace SSGE
{
    auto CSharpCompiler::compile(const std::filesystem::path& projectPath,
                                 const std::string& projectName) -> std::string
    {
        std::filesystem::path dotNetProjectLocation =
            projectPath / projectName / std::format("{}.csproj", projectName);

        auto command = std::format("dotnet build \"{}\" -c Debug", dotNetProjectLocation.string());

        if (std::system(command.c_str()))
        {
            return "Failed to build project: " + projectName;
        }

        auto dllName = std::format("{}.dll", projectName);
        auto depsJsonName = std::format("{}.deps.json", projectName);
        auto pdbName = std::format("{}.pdb", projectName);

        std::filesystem::copy_file(projectPath / projectName / "bin" / "Debug" / dllName,
                                   std::format("./{}", dllName), std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy_file(projectPath / projectName / "bin" / "Debug" / depsJsonName,
                                   std::format("./{}", depsJsonName),
                                   std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy_file(projectPath / projectName / "bin" / "Debug" / pdbName,
                                   std::format("./{}", pdbName), std::filesystem::copy_options::overwrite_existing);

        return {};
    }
} // SSGE
