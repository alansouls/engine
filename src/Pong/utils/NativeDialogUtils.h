#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

namespace SSGE::Editor
{
class NativeDialogUtils
{
  public:
    static auto OpenReadFileFromDialog(const std::string &filterName, const std::string &allowedExtensions)
        -> std::optional<std::ifstream>;
    static auto OpenSaveFileFromDialog(const std::string &filterName, const std::string &allowedExtensions)
        -> std::optional<std::ofstream>;
    static auto GetReadFileFromDialog(const std::string &filterName, const std::string &allowedExtensions)
        -> std::optional<std::filesystem::path>;
    static auto GetSaveFileFromDialog(const std::string &filterName, const std::string &allowedExtensions)
        -> std::optional<std::filesystem::path>;
};
} // namespace SSGE::Editor
