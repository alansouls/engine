//
// Created by Alan Maia on 01/02/2026.
//

#include "ComponentField.h"
#include "scenes/Game.h"

#include <format>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <utility>

namespace SSGE
{

ComponentField::ComponentField(std::string name, FieldType type) : m_name(std::move(name)), m_type(type)
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

template <ComponentFieldDataType TDataType>
TypedComponentField<TDataType>::TypedComponentField(std::string name, FieldType type, std::function<TDataType()> getter,
                                                    std::function<void(const TDataType &)> setter)
    : ComponentField(std::move(name), type), m_initialValue(getter()), m_getter(std::move(getter)),
      m_setter(std::move(setter))
{
}

template <ComponentFieldDataType TDataType> auto TypedComponentField<TDataType>::currentValue() -> TDataType
{
    return m_getter();
}

template <ComponentFieldDataType TDataType>
auto TypedComponentField<TDataType>::setCurrentValue(const TDataType &value) -> void
{
    m_setter(value);

    if (!Game::getInstance()->isStarted())
    {
        m_initialValue = value;
    }
}

template <ComponentFieldDataType TDataType> auto TypedComponentField<TDataType>::applyInitialValue() -> void
{
    m_setter(m_initialValue);
}

template <> auto TypedComponentField<int>::valueAsString() const -> std::string
{
    return std::format("{}", m_getter());
}

template <> auto TypedComponentField<float>::valueAsString() const -> std::string
{
    return std::format("{:.6f}", m_getter());
}

template <> auto TypedComponentField<bool>::valueAsString() const -> std::string
{
    return m_getter() ? "T" : "";
}

template <> auto TypedComponentField<std::string>::valueAsString() const -> std::string
{
    return m_getter();
}

template <> auto TypedComponentField<glm::vec2>::valueAsString() const -> std::string
{
    glm::vec2 vec = m_getter();
    return std::format("{:.6f}|{:.6f}", vec.x, vec.y);
}

template <> auto TypedComponentField<glm::vec3>::valueAsString() const -> std::string
{
    glm::vec3 vec = m_getter();
    return std::format("{:.6f}|{:.6f}|{:.6f}", vec.x, vec.y, vec.z);
}

template <> auto TypedComponentField<glm::vec4>::valueAsString() const -> std::string
{
    glm::vec4 vec = m_getter();
    return std::format("{:.6f}|{:.6f}|{:.6f}|{:.6f}", vec.r, vec.g, vec.b, vec.a);
}

template class TypedComponentField<int>;
template class TypedComponentField<float>;
template class TypedComponentField<bool>;
template class TypedComponentField<std::string>;
template class TypedComponentField<glm::vec2>;
template class TypedComponentField<glm::vec3>;
template class TypedComponentField<glm::vec4>;

} // namespace SSGE
