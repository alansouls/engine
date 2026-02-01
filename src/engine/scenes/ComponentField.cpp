//
// Created by Alan Maia on 01/02/2026.
//

#include "ComponentField.h"

#include <charconv>
#include <format>
#include <utility>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace SSGE
{
    ComponentField::ComponentField(std::string name, std::string value, FieldType type, void* dataRef) :
        m_name(std::move(name)),
        m_value(std::move(value)), m_type(type),
        m_dataRef(dataRef)
    {
    }

    auto ComponentField::apply() const -> void
    {
        switch (m_type)
        {
        case Int:
            *static_cast<int*>(m_dataRef) = std::stoi(m_value);
            break;
        case Float:
            *static_cast<float*>(m_dataRef) = std::stof(m_value);
            break;
        case String:
            *static_cast<std::string*>(m_dataRef) = m_value;
            break;
        case Bool:
            *static_cast<bool*>(m_dataRef) = m_value == "true";
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

    auto strSplit(const std::string& str, const std::string& delimiter) -> std::vector<std::string_view>
    {
        std::vector<std::string_view> result;
        auto pos = str.find(delimiter);
        size_t offset = 0;
        const char* rawStr = str.c_str();
        while (pos != std::string::npos)
        {
            result.emplace_back(rawStr + offset, pos - offset);
            offset = pos + delimiter.length();
            pos = str.find(delimiter, pos + delimiter.length());
        }
        result.emplace_back(rawStr + offset);
        return result;
    }

    auto strToFloat(const std::string_view& str) -> float
    {
        float result;
        std::from_chars(str.data(), str.data() + str.size(), result);

        return result;
    }

    auto ComponentField::applyVec2() const -> void
    {
        auto values = strSplit(m_value, "|");
        if (values.size() != 2)
        {
            throw std::runtime_error(std::format("Unexpected format, expect 2 values separate by '|' but found {}",
                                                 values.size()));
        }

        float x = strToFloat(values[0]);
        float y = strToFloat(values[1]);

        *static_cast<glm::vec2*>(m_dataRef) = glm::vec2(x, y);
    }

    auto ComponentField::applyVec3() const -> void
    {
        auto values = strSplit(m_value, "|");
        if (values.size() != 3)
        {
            throw std::runtime_error(std::format("Unexpected format, expect 3 values separate by '|' but found {}",
                                                 values.size()));
        }

        float x = strToFloat(values[0]);
        float y = strToFloat(values[1]);
        float z = strToFloat(values[2]);

        *static_cast<glm::vec3*>(m_dataRef) = glm::vec3(x, y, z);
    }

    auto ComponentField::applyVec4() const -> void
    {
        auto values = strSplit(m_value, "|");
        if (values.size() != 4)
        {
            throw std::runtime_error(std::format("Unexpected format, expect 4 values separate by '|' but found {}",
                                                 values.size()));
        }

        float x = strToFloat(values[0]);
        float y = strToFloat(values[1]);
        float z = strToFloat(values[2]);
        float a = strToFloat(values[3]);

        *static_cast<glm::vec4*>(m_dataRef) = glm::vec4(x, y, z, a);
    }
} // SSGE
