#include "ParseUtils.h"

#include "StringUtils.h"

#include <charconv>
#include <format>

namespace SSGE
{

template <typename TNumber> auto parseNumber(const std::string_view &value) -> TNumber
{
    TNumber number;
    std::from_chars(value.data(), value.data() + value.size(), number);
    return number;
}

template <> auto ParseUtils::Parse<int>(const std::string_view &value) -> int
{
    return parseNumber<int>(value);
}

template <> auto ParseUtils::Parse<float>(const std::string_view &value) -> float
{
    return parseNumber<float>(value);
}

template <> auto ParseUtils::Parse<bool>(const std::string_view &value) -> bool
{
    return value[0] == TrueChar;
}

template <> auto ParseUtils::Parse<std::string>(const std::string_view &value) -> std::string
{
    return std::string(value);
}

template <size_t Dimensions> auto parseVector(const std::string_view &value) -> glm::vec<Dimensions, float>
{
    std::vector<std::string_view> parts = StringUtils::Split(value, "|");

    if (parts.size() != Dimensions)
    {
        throw std::runtime_error(std::format("Could not parse Vector {} from '{}'", Dimensions, value));
    }

    glm::vec<Dimensions, float> result{};

    for (size_t i = 0; i < Dimensions; ++i)
    {
        result[i] = parseNumber<float>(parts[i]);
    }

    return result;
}

template <> auto ParseUtils::Parse<glm::vec2>(const std::string_view &value) -> glm::vec2
{
    return parseVector<2>(value);
}

template <> auto ParseUtils::Parse<glm::vec3>(const std::string_view &value) -> glm::vec3
{
    return parseVector<3>(value);
}

template <> auto ParseUtils::Parse<glm::vec4>(const std::string_view &value) -> glm::vec4
{
    return parseVector<4>(value);
}
} // namespace SSGE