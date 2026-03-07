#pragma once

#include <optional>
#include <string>
#include <fstream>

namespace SSGE::Editor
{
class NativeDialogUtils
{
public:
    static auto OpenReadFileFromDialog(const std::string &filterName, const std::string &allowedExtensions) -> std::optional<std::ifstream>;
    static auto OpenSaveFileFromDialog(const std::string &filterName, const std::string &allowedExtensions) -> std::optional<std::ofstream>;
};
} // SSGE