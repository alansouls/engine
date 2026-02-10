//
// Created by Alan Maia on 01/02/2026.
//

#include "ComponentField.h"

#include <charconv>
#include <format>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <utility>
#include <vector>

namespace SSGE
{
ComponentField::ComponentField(std::string name, std::string value, FieldType type, void *dataRef)
    : m_name(std::move(name)), m_value(std::move(value)), m_type(type), m_dataRef(dataRef)
{
}

auto ComponentField::apply() const -> void
{
    switch (m_type)
    {
    case Int:
        *static_cast<int *>(m_dataRef) = std::stoi(m_value);
        break;
    case Float:
        *static_cast<float *>(m_dataRef) = std::stof(m_value);
        break;
    case String:
        *static_cast<std::string *>(m_dataRef) = m_value;
        break;
    case Bool:
        *static_cast<bool *>(m_dataRef) = m_value == "true";
        break;
    case Vec2:
        applyVec2();
        break;
    case Vec3:
        applyVec3();
        break;
    case Vec4:
    case Color:
        applyVec4();
        break;
    default:
        throw std::runtime_error(std::format("Unknown component type {}", static_cast<int>(m_type)));
    }
}

auto ComponentField::setValue(std::string value) -> void
{
    m_value = std::move(value);
}

auto ComponentField::name() const -> const std::string &
{
    return m_name;
}

auto ComponentField::value() const -> const std::string &
{
    return m_value;
}

auto ComponentField::type() const -> FieldType
{
    return m_type;
}

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

auto ComponentField::vec2ToString(glm::vec2 value) -> std::string
{
    return std::format("{}|{}", value.x, value.y);
}

auto ComponentField::stringToVec2(const std::string &value) -> glm::vec2
{
    auto values = strSplit(value, "|");
    if (values.size() != 2)
    {
        throw std::runtime_error(
            std::format("Unexpected format, expect 2 values separate by '|' but found {}", values.size()));
    }

    float x = strToFloat(values[0]);
    float y = strToFloat(values[1]);

    return {x, y};
}

auto ComponentField::vec3ToString(glm::vec3 value) -> std::string
{
    return std::format("{}|{}|{}", value.x, value.y, value.z);
}

auto ComponentField::stringToVec3(const std::string &value) -> glm::vec3
{
    auto values = strSplit(value, "|");
    if (values.size() != 3)
    {
        throw std::runtime_error(
            std::format("Unexpected format, expect 3 values separate by '|' but found {}", values.size()));
    }

    float x = strToFloat(values[0]);
    float y = strToFloat(values[1]);
    float z = strToFloat(values[2]);

    return {x, y, z};
}

auto ComponentField::vec4ToString(glm::vec4 value) -> std::string
{
    return std::format("{}|{}|{}|{}", value.x, value.y, value.z, value.w);
}

auto ComponentField::stringToVec4(const std::string &value) -> glm::vec4
{
    auto values = strSplit(value, "|");
    if (values.size() != 4)
    {
        throw std::runtime_error(
            std::format("Unexpected format, expect 4 values separate by '|' but found {}", values.size()));
    }

    float x = strToFloat(values[0]);
    float y = strToFloat(values[1]);
    float z = strToFloat(values[2]);
    float w = strToFloat(values[3]);

    return {x, y, z, w};
}

auto ComponentField::applyVec2() const -> void
{
    *static_cast<glm::vec2 *>(m_dataRef) = stringToVec2(m_value);
}

auto ComponentField::applyVec3() const -> void
{
    *static_cast<glm::vec3 *>(m_dataRef) = stringToVec3(m_value);
}

auto ComponentField::applyVec4() const -> void
{
    *static_cast<glm::vec4 *>(m_dataRef) = stringToVec4(m_value);
}
} // namespace SSGE
