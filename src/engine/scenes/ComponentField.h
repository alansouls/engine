#pragma once
#include <functional>
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

        ComponentField(std::string name, std::string value, FieldType type, void* dataRef);

        auto apply() const  -> void;

        auto setValue(std::string value) -> void;

    private:
        std::string m_name;
        std::string m_value;
        FieldType m_type;
        void* m_dataRef;

        auto applyVec2() const -> void;
        auto applyVec3() const -> void;
        auto applyVec4() const -> void;
    };
} // SSGE
