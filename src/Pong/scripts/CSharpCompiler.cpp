//
// Created by Alan Maia on 31/01/2026.
//

#include "CSharpCompiler.h"

#include <filesystem>
#include <format>
#include <thread>

namespace SSGE
{

std::atomic<bool> CSharpCompiler::s_compiling{false};
std::string CSharpCompiler::s_latestResult;

auto CSharpCompiler::compile(const std::filesystem::path &projectPath, const std::string &projectName) -> std::string
{
    if (s_compiling.load())
    {
        return "The project is already being compiled";
    }

    std::thread compileThread(CSharpCompiler::compilePrivate, projectPath, projectName);

    compileThread.detach();

    return "";
}

auto CSharpCompiler::isCompiling() -> bool
{
    return s_compiling.load();
}

auto CSharpCompiler::compilePrivate(const std::filesystem::path &projectPath, const std::string &projectName) -> void
{
    s_compiling.store(true);
    std::filesystem::path dotNetProjectLocation = projectPath / projectName / std::format("{}.csproj", projectName);

    auto command = std::format("dotnet build \"{}\" -c Debug", dotNetProjectLocation.string());

    if (std::system(command.c_str()))
    {
        s_latestResult = "Failed to build project: " + projectName;
    }

    auto dllName = std::format("{}.dll", projectName);
    auto depsJsonName = std::format("{}.deps.json", projectName);
    auto pdbName = std::format("{}.pdb", projectName);

    std::filesystem::copy_file(projectPath / projectName / "bin" / "Debug" / dllName, std::format("./{}", dllName),
                               std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(projectPath / projectName / "bin" / "Debug" / depsJsonName,
                               std::format("./{}", depsJsonName), std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(projectPath / projectName / "bin" / "Debug" / pdbName, std::format("./{}", pdbName),
                               std::filesystem::copy_options::overwrite_existing);

    s_latestResult = "";
    s_compiling.store(false);
}
} // namespace SSGE
