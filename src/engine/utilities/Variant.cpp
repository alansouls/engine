#include "Variant.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>

namespace SSGE
{

template <> auto Variant::getVariantType<int>() -> VariantType
{
    return VariantType::Int;
}

template <> auto Variant::getVariantType<float>() -> VariantType
{
    return VariantType::Float;
}

template <> auto Variant::getVariantType<bool>() -> VariantType
{
    return VariantType::Bool;
}

template <> auto Variant::getVariantType<std::string>() -> VariantType
{
    return VariantType::String;
}

template <> auto Variant::getVariantType<glm::vec2>() -> VariantType
{
    return VariantType::Vec2;
}

template <> auto Variant::getVariantType<glm::vec3>() -> VariantType
{
    return VariantType::Vec3;
}

template <> auto Variant::getVariantType<glm::vec4>() -> VariantType
{
    return VariantType::Vec4;
}

Variant::Variant() : m_dataPtr(nullptr)
{
}

template <typename TDataType> Variant::Variant(const TDataType &data)
{
    initDataPtr(data);
    m_type = getVariantType<TDataType>();
}

template <typename TDataType> Variant::Variant(TDataType *data)
{
    m_dataPtr = data;
    m_type = getVariantType<TDataType>();
}

template <typename TDataType> auto Variant::getValue() -> const TDataType &
{
    if (!m_dataPtr)
        return TDataType();

    return *reinterpret_cast<TDataType *>(m_dataPtr);
}

template <typename TDataType> auto Variant::setValue(const TDataType &value) -> void
{
    if (m_dataPtr)
        *reinterpret_cast<TDataType *>(m_dataPtr) = value;
    else
        getVariantType(value);
}

} // namespace SSGE
