#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>

namespace SSGE
{
class ComponentField
{
  public:
    enum FieldType
    {
        Int,
        Float,
        String,
        Bool,
        Vec2,
        Vec3,
        Vec4,
        Color,
        FieldTypeCount
    };

    ComponentField(std::string name, std::string value, FieldType type, void *dataRef);

    auto apply() const -> void;

    auto setValue(std::string value) -> void;

    [[nodiscard]] auto name() const -> const std::string &;

    [[nodiscard]] auto value() const -> const std::string &;

    [[nodiscard]] auto type() const -> FieldType;

    static auto vec2ToString(glm::vec2 value) -> std::string;
    static auto stringToVec2(const std::string &value) -> glm::vec2;
    static auto vec3ToString(glm::vec3 value) -> std::string;
    static auto stringToVec3(const std::string &value) -> glm::vec3;
    static auto vec4ToString(glm::vec4 value) -> std::string;
    static auto stringToVec4(const std::string &value) -> glm::vec4;

  private:
    std::string m_name;
    std::string m_value;
    FieldType m_type;
    void *m_dataRef;

    auto applyVec2() const -> void;
    auto applyVec3() const -> void;
    auto applyVec4() const -> void;
};
} // namespace SSGE
