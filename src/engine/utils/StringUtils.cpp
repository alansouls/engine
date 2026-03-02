//
// Created by Alan Maia on 25/02/2026.
//

#include "StringUtils.h"

#include <charconv>

namespace SSGE
{

auto StringUtils::Split(const std::string_view &str, const std::string &delimiter) -> std::vector<std::string_view>
{
    std::vector<std::string_view> result;
    auto pos = str.find(delimiter);
    size_t offset = 0;
    const char *rawStr = str.data();
    while (pos != std::string::npos)
    {
        result.emplace_back(rawStr + offset, pos - offset);
        offset = pos + delimiter.length();
        pos = str.find(delimiter, pos + delimiter.length());
    }
    result.emplace_back(rawStr + offset, str.size() - offset);
    return result;
}

auto StringUtils::ViewToFloat(const std::string_view &str) -> float
{
    float result;
    std::from_chars(str.data(), str.data() + str.size(), result);

    return result;
}
} // namespace SSGE