#pragma once

#include <functional>

namespace SSGE
{
class Variant
{
  public:
    enum VariantType
    {
        Int,
        Float,
        String,
        Bool,
        Vec2,
        Vec3,
        Vec4
    };

    Variant();
    Variant(const Variant &) = delete;
    template <typename TDataType> Variant(const TDataType &data);
    template <typename TDataType> Variant(TDataType *data);
    ~Variant()
    {
        if (m_deleteFunc)
            m_deleteFunc(m_dataPtr);
    }

    template <typename TDataType> auto getValue() -> const TDataType &;

    template <typename TDataType> auto setValue(const TDataType &value) -> void;

  private:
    void *m_dataPtr;
    std::function<void(void *)> m_deleteFunc;
    VariantType m_type;

    template <typename TDataType> auto getVariantType() -> VariantType;

    template <typename TDataType> auto initDataPtr(const TDataType &value) -> void *;
};

template <typename TDataType> auto Variant::initDataPtr(const TDataType &value) -> void *
{
    TDataType *dataPtr = new TDataType();
    *dataPtr = value;
    m_dataPtr = dataPtr;
    m_deleteFunc = [](void *rawPtr) { delete reinterpret_cast<TDataType *>(rawPtr); };
}

} // namespace SSGE
