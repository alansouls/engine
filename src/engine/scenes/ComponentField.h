#pragma once
#include "utilities/Variant.h"
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

    ComponentField(std::string name, FieldType type, void *dataRef);

    template <typename fieldType> auto currentValue() -> fieldType;

    template <typename fieldType> auto setCurrentValue(const fieldType &value) -> void;

    auto applyInitialValue() -> void;

    [[nodiscard]] auto name() const -> const std::string &;

    [[nodiscard]] auto type() const -> FieldType;

  private:
    std::string m_name;
    FieldType m_type;
    Variant m_value;
    Variant m_initialValue;

    auto apply(const std::string &value) const -> void;
    auto applyVec2(const std::string &value) const -> void;
    auto applyVec3(const std::string &value) const -> void;
    auto applyVec4(const std::string &value) const -> void;
};
} // namespace SSGE
