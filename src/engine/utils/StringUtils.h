#pragma once
#include <string>
#include <vector>

namespace SSGE
{

class StringUtils
{
  public:
    static auto Split(const std::string_view &str, const std::string &delimiter) -> std::vector<std::string_view>;

    static auto ViewToFloat(const std::string_view &str) -> float;
};

} // namespace SSGE
