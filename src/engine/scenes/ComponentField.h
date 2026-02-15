#pragma once
#include <concepts>
#include <functional>
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

    ComponentField(std::string name, FieldType type);
    virtual ~ComponentField() = default;

    virtual auto applyInitialValue() -> void = 0;

    [[nodiscard]] auto name() const -> const std::string &;

    [[nodiscard]] auto type() const -> FieldType;

  private:
    std::string m_name;
    FieldType m_type;
};

template <typename TDataType>
concept ComponentFieldDataType =
    std::same_as<TDataType, int> || std::same_as<TDataType, float> || std::same_as<TDataType, std::string> ||
    std::same_as<TDataType, bool> || std::same_as<TDataType, glm::vec2> || std::same_as<TDataType, glm::vec3> ||
    std::same_as<TDataType, glm::vec4>;

template <typename TDataType> class TypedComponentField : public ComponentField
{
  public:
    TypedComponentField(std::string name, FieldType type, std::function<TDataType()> getter,
                        std::function<void(const TDataType &)> setter);
    ~TypedComponentField() override = default;

    auto currentValue() -> TDataType;

    auto setCurrentValue(const TDataType &value) -> void;

    auto applyInitialValue() -> void override;

  private:
    TDataType m_initialValue;
    std::function<TDataType()> m_getter;
    std::function<void(const TDataType &)> m_setter;
};

} // namespace SSGE
