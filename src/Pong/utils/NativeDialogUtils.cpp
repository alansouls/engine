#include "nfd.h"

#include "NativeDialogUtils.h"

#include "scenes/Game.h"
#include <filesystem>
#include <optional>

namespace SSGE::Editor
{

auto nfdGetOutPath(const std::string &filterName, const std::string &allowedExtensions) -> std::string
{
    nfdu8char_t *outPath;
    nfdu8filteritem_t filter = {filterName.c_str(), allowedExtensions.c_str()};
    nfdopendialogu8args_t args = {};
    args.filterList = &filter;
    args.filterCount = 1;
    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
    if (result == NFD_OKAY)
    {
        std::string str(outPath);
        NFD_FreePathU8(outPath);
        return str;
    }
    if (result == NFD_CANCEL)
    {
        return "";
    }

    throw std::runtime_error("Couldn't open file dialog");
}

auto nfdGetOutPathForSave(const std::string &filterName, const std::string &allowedExtensions) -> std::string
{
    nfdu8char_t *outPath;
    nfdu8filteritem_t filter = {filterName.c_str(), allowedExtensions.c_str()};
    nfdsavedialogu8args_t args = {};
    args.filterList = &filter;
    args.filterCount = 1;
    nfdresult_t result = NFD_SaveDialogU8_With(&outPath, &args);
    if (result == NFD_OKAY)
    {
        std::string str(outPath);
        NFD_FreePathU8(outPath);
        return str;
    }
    if (result == NFD_CANCEL)
    {
        return "";
    }

    throw std::runtime_error("Couldn't open file dialog");
}

auto NativeDialogUtils::OpenReadFileFromDialog(const std::string &filterName, const std::string &allowedExtensions)
    -> std::optional<std::ifstream>
{
    std::string filePath = nfdGetOutPath(filterName, allowedExtensions);

    if (filePath.empty())
    {
        return std::nullopt;
    }

    return std::ifstream(filePath, std::ios::in | std::ios::binary);
}

auto NativeDialogUtils::OpenSaveFileFromDialog(const std::string &filterName, const std::string &allowedExtensions)
    -> std::optional<std::ofstream>
{
    std::string filePath = nfdGetOutPathForSave(filterName, allowedExtensions);

    if (filePath.empty())
    {
        return std::nullopt;
    }

    return std::ofstream(filePath, std::ios::trunc | std::ios::out | std::ios::binary);
}

auto NativeDialogUtils::GetReadFileFromDialog(const std::string &filterName, const std::string &allowedExtensions)
    -> std::optional<std::filesystem::path>
{
    std::string pathStr = nfdGetOutPath(filterName, allowedExtensions);
    return pathStr.empty() ? std::nullopt : std::optional(std::filesystem::path(pathStr));
}

auto NativeDialogUtils::GetSaveFileFromDialog(const std::string &filterName, const std::string &allowedExtensions)
    -> std::optional<std::filesystem::path>
{
    std::string pathStr = nfdGetOutPathForSave(filterName, allowedExtensions);
    return pathStr.empty() ? std::nullopt : std::optional(std::filesystem::path(pathStr));
}

auto NativeDialogUtils::GetFolderFromDialog() -> std::optional<std::filesystem::path>
{
    nfdu8char_t *outPath;
    nfdpickfolderu8args_t args = {};
    nfdresult_t result = NFD_PickFolderU8_With(&outPath, &args);
    if (result == NFD_OKAY)
    {
        std::filesystem::path path(outPath);
        NFD_FreePathU8(outPath);
        return path;
    }
    if (result == NFD_CANCEL)
    {
        return std::nullopt;
    }
    throw std::runtime_error("Couldn't open folder dialog");
}

} // namespace SSGE::Editor
