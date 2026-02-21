#include "ScriptComponent.h"

#include "scenes/Game.h"
#include "scenes/GameObject.h"
#include "scripts/CSharpExecutionEngine.h"

#include <utility>

namespace SSGE
{
ScriptComponent::ScriptComponent(GameObject *gameObject, std::string className)
    : Component(className, gameObject), m_className(std::move(className)),
      m_scriptRunnerParameter{.gameObject = gameObject, .scriptName = m_className.c_str()}
{
}

auto ScriptComponent::init() -> void
{
    commitProperties();

    auto engine = CSharpExecutionEngine::Get();

    component_entry_point_fn initFunction = engine->getComponentEntryPointFunctions()[0];

    if (initFunction == nullptr)
    {
        return;
    }

    initFunction(&m_scriptRunnerParameter, sizeof(ScriptRunnerParameter));
}

auto ScriptComponent::update() -> void
{
    commitProperties();

    auto engine = CSharpExecutionEngine::Get();

    component_entry_point_fn updateFunction = engine->getComponentEntryPointFunctions()[1];

    if (updateFunction == nullptr)
    {
        return;
    }

    updateFunction(&m_scriptRunnerParameter, sizeof(ScriptRunnerParameter));
}

auto ScriptComponent::updateFields(const ScriptComponentInfo &info) -> void
{
    m_fields.clear();

    for (const auto &property : info.Properties)
    {
        m_fields.push_back(makeFieldForComponentProperty(property));
    }
}

#define MAKE_COMPONENT_FIELD(CType, EngineType) std::make_unique<TypedComponentField<CType>>(propertyInfo.Name, EngineType, [this, propertyInfo] { \
return getManagedProperty<CType>(propertyInfo.Name); \
}, [this, propertyInfo](const CType &value) { \
    setManagedProperty(propertyInfo.Name, value); \
}); \

auto ScriptComponent::makeFieldForComponentProperty(const ComponentPropertyInfo &propertyInfo) -> std::unique_ptr<ComponentField>
{
    if (propertyInfo.Type == "Boolean")
    {
        return MAKE_COMPONENT_FIELD(bool, ComponentField::FieldType::Bool);
    }
    if (propertyInfo.Type == "Int32")
    {
        return MAKE_COMPONENT_FIELD(int, ComponentField::FieldType::Int);
    }
    if (propertyInfo.Type == "Float32")
    {
        return MAKE_COMPONENT_FIELD(float, ComponentField::FieldType::Float);
    }
    if (propertyInfo.Type == "Vec2")
    {
        return MAKE_COMPONENT_FIELD(glm::vec2, ComponentField::FieldType::Vec2);
    }
    if (propertyInfo.Type == "Vec3")
    {
        return MAKE_COMPONENT_FIELD(glm::vec3, ComponentField::FieldType::Vec3);
    }
    if (propertyInfo.Type == "Vec4")
    {
        return MAKE_COMPONENT_FIELD(glm::vec4, ComponentField::FieldType::Vec4);
    }
    if (propertyInfo.Type == "String")
    {
        return MAKE_COMPONENT_FIELD(std::string, ComponentField::FieldType::String);
    }
    if (propertyInfo.Type == "Color")
    {
        return MAKE_COMPONENT_FIELD(glm::vec4, ComponentField::FieldType::Color);
    }

    throw std::runtime_error("Unsupported component property type");
}

template <ComponentFieldDataType T> auto ScriptComponent::getManagedProperty(const std::string &propertyName) -> T
{
    if (!Game::getInstance()->isGameAssemblyLoaded())
    {
        auto &variantValue = m_managedPropertyValues[propertyName];

        if (variantValue.index() == 0) // std::monostate
        {
            variantValue = T();
        }

        return std::get<T>(variantValue);
    }

    CSharpExecutionEngine *engine = CSharpExecutionEngine::Get();

    component_entry_point_fn function = engine->getComponentEntryPointFunctions()[CSharpExecutionEngine::GetProperty];

    T value;
    struct
    {
        GameObject *gameObject;
        ScriptComponent *component;
        const char* propertyName;
        T* valuePtr;
    } getManagedPropertyParameters {.propertyName = propertyName.c_str(), .valuePtr = &value};

    if (function(&getManagedPropertyParameters, sizeof(getManagedPropertyParameters)))
    {
        throw std::runtime_error(std::format("Error getting property {} from script component {}", propertyName, m_className));
    }

    return value;
}

template <ComponentFieldDataType T>
auto ScriptComponent::setManagedProperty(const std::string &propertyName, const T &data) -> void
{
    if (!Game::getInstance()->isGameAssemblyLoaded())
    {
        m_managedPropertyValues[propertyName] = data;
        return;
    }

    CSharpExecutionEngine *engine = CSharpExecutionEngine::Get();

    component_entry_point_fn function = engine->getComponentEntryPointFunctions()[CSharpExecutionEngine::SetProperty];

    struct
    {
        GameObject *gameObject;
        ScriptComponent *component;
        const char* propertyName;
        const T* valuePtr;
    } setManagedPropertyParameters {.propertyName = propertyName.c_str(), .valuePtr = &data};

    if (function(&setManagedPropertyParameters, sizeof(setManagedPropertyParameters)))
    {
        throw std::runtime_error(std::format("Error setting property {} from script component {}", propertyName, m_className));
    }
}

auto ScriptComponent::commitProperties() -> void
{
    // TODO optimize this passing all properties at once
    // for (auto &[name, value] : m_pendingProperties)
    // {
    //     setPropertyManaged(name, value);
    // }
    //
    // m_pendingProperties.clear();
}

auto ScriptComponent::setPropertyManaged(const std::string &propertyName, const std::string &propertyValue) const
    -> void
{
    SetPropertiesParameter parameter = {.gameObject = m_scriptRunnerParameter.gameObject,
                                        .scriptName = m_scriptRunnerParameter.scriptName,
                                        .propertyName = propertyName.c_str(),
                                        .propertyValue = propertyValue.c_str()};

    component_entry_point_fn setPropertyFunction = CSharpExecutionEngine::Get()->getComponentEntryPointFunctions()[2];

    if (setPropertyFunction == nullptr)
    {
        return;
    }

    setPropertyFunction(&parameter, sizeof(parameter));
}

} // namespace SSGE