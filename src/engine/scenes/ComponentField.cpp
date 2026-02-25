//
// Created by Alan Maia on 01/02/2026.
//

#include "ComponentField.h"
#include "scenes/Game.h"

#include <charconv>
#include <cstdlib>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <utility>
#include <vector>

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
    : ComponentField(std::move(name), type), m_initialValue(getter()), m_getter(std::move(getter)), m_setter(std::move(setter))
{
}

template <ComponentFieldDataType TDataType> auto TypedComponentField<TDataType>::currentValue() -> TDataType
{
    return m_getter();
}

template <ComponentFieldDataType TDataType> auto TypedComponentField<TDataType>::setCurrentValue(const TDataType &value) -> void
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

template class TypedComponentField<int>;
template class TypedComponentField<float>;
template class TypedComponentField<bool>;
template class TypedComponentField<std::string>;
template class TypedComponentField<glm::vec2>;
template class TypedComponentField<glm::vec3>;
template class TypedComponentField<glm::vec4>;

} // namespace SSGE
