//
// Created by Alan Maia on 01/02/2026.
//

#include "ComponentField.h"
#include "scenes/Game.h"

#include <charconv>
#include <cstdlib>
#include <format>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stdexcept>
#include <utility>
#include <vector>

namespace SSGE
{

ComponentField::ComponentField(std::string name, FieldType type, void *dataRef)
    : m_name(std::move(name)), m_type(type), m_value(dataRef)
{
    switch (type)
    {
    case Int: {
        auto initialValue = new int();
        *initialValue = *static_cast<int *>(dataRef);
    }
    break;
    case Float: {
        auto initialValue = new float();
        *initialValue = *static_cast<float *>(dataRef);
    }
    break;
    case String:
    case Bool:
    case Vec2:
    case Vec3:
    case Vec4:
    case Color:
    default:
        throw std::runtime_error("Unexpected type");
        break;
    }
}

auto ComponentField::applyInitialValue() -> void
{
}

auto ComponentField::name() const -> const std::string &
{
    return m_name;
}

auto ComponentField::type() const -> FieldType
{
    return m_type;
}

// TOOD: move the functions bellow to a string utils file
auto strSplit(const std::string &str, const std::string &delimiter) -> std::vector<std::string_view>
{
    std::vector<std::string_view> result;
    auto pos = str.find(delimiter);
    size_t offset = 0;
    const char *rawStr = str.c_str();
    while (pos != std::string::npos)
    {
        result.emplace_back(rawStr + offset, pos - offset);
        offset = pos + delimiter.length();
        pos = str.find(delimiter, pos + delimiter.length());
    }
    result.emplace_back(rawStr + offset);
    return result;
}

auto strToFloat(const std::string_view &str) -> float
{
    float result;
    std::from_chars(str.data(), str.data() + str.size(), result);

    return result;
}

} // namespace SSGE
