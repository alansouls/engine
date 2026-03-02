#pragma once
#include <atomic>
#include <filesystem>
#include <functional>
#include <string>
#include <thread>

namespace SSGE
{
class CSharpCompiler
{
public:
    CSharpCompiler() = delete;
    static auto compile(const std::filesystem::path &projectPath, const std::string &projectName) -> std::string;
    static auto compile(const std::filesystem::path &projectPath, const std::string &projectName,
                        const std::function<void ()> &continueWith) -> std::string;

    [[nodiscard]] static auto isCompiling() -> bool;

private:
    static std::atomic<bool> s_compiling;
    static std::string s_latestResult;

    static auto compilePrivate(const std::filesystem::path &projectPath, const std::string &projectName) -> void;
};
} // namespace SSGE