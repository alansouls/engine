//
// Created by Alan Maia on 01/02/2026.
//

#include "ComponentField.h"
#include "scenes/Game.h"

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
    : m_name(std::move(name)), m_initialValue(value), m_currentValue(std::move(value)), m_type(type), m_dataRef(dataRef)
{
}

auto ComponentField::activeValue() -> std::string
{
    auto game = Game::getInstance();

    return game->isStarted() ? m_currentValue : m_initialValue;
}

auto ComponentField::setActiveValue(const std::string &value) -> void
{
    auto game = Game::getInstance();

    if (game->isStarted())
    {
        setCurrentValue(value);
    }
    else
    {
        setInitialValue(value);
    }
}

auto ComponentField::applyActiveValue() -> void
{
    auto game = Game::getInstance();

    if (game->isStarted())
    {
        applyCurrentValue();
    }
    else
    {
        applyInitialValue();
    }
}

auto ComponentField::applyInitialValue() -> void
{
    apply(m_initialValue);
}

auto ComponentField::applyCurrentValue() -> void
{
    apply(m_currentValue);
}

auto ComponentField::apply(const std::string &value) const -> void
{
    switch (m_type)
    {
    case Int:
        *static_cast<int *>(m_dataRef) = std::stoi(value);
        break;
    case Float:
        *static_cast<float *>(m_dataRef) = std::stof(value);
        break;
    case String:
        *static_cast<std::string *>(m_dataRef) = value;
        break;
    case Bool:
        *static_cast<bool *>(m_dataRef) = value == "true";
        break;
    case Vec2:
        applyVec2(value);
        break;
    case Vec3:
        applyVec3(value);
        break;
    case Vec4:
    case Color:
        applyVec4(value);
        break;
    default:
        throw std::runtime_error(std::format("Unknown component type {}", static_cast<int>(m_type)));
    }
}

auto ComponentField::setInitialValue(std::string value) -> void
{
    m_initialValue = std::move(value);
}

auto ComponentField::setCurrentValue(std::string value) -> void
{
    m_currentValue = std::move(value);
}

auto ComponentField::name() const -> const std::string &
{
    return m_name;
}

auto ComponentField::initialValue() const -> const std::string &
{
    return m_currentValue;
}

auto ComponentField::currentValue() const -> const std::string &
{
    return m_currentValue;
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

auto ComponentField::applyVec2(const std::string &value) const -> void
{
    *static_cast<glm::vec2 *>(m_dataRef) = stringToVec2(value);
}

auto ComponentField::applyVec3(const std::string &value) const -> void
{
    *static_cast<glm::vec3 *>(m_dataRef) = stringToVec3(value);
}

auto ComponentField::applyVec4(const std::string &value) const -> void
{
    *static_cast<glm::vec4 *>(m_dataRef) = stringToVec4(value);
}
} // namespace SSGE
