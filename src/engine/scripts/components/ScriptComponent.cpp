#include "ScriptComponent.h"

#include "scenes/Game.h"
#include "scenes/GameObject.h"
#include "scripts/CSharpExecutionEngine.h"

#include <limits>
#include <stdexcept>
#include <utility>

#include <iostream>

namespace SSGE
{

static auto classNameFromFullName(const std::string &fullName) -> std::string
{
    if (fullName.empty())
    {
        return fullName;
    }

    size_t start;
    for (start = fullName.length() - 1; start > 0; --start)
    {
        if (fullName[start] == '.')
        {
            start = start + 1;
            break;
        }
    }

    if (start == fullName.length())
    {
        throw std::runtime_error("Invalid full class name for script component!");
    }

    return fullName.substr(start);
}

ScriptComponent::ScriptComponent(GameObject *gameObject, std::string fullClassName)
    : Component(fullClassName, classNameFromFullName(fullClassName), gameObject, Component::ComponentType::Script),
      m_className(std::move(fullClassName)),
      m_scriptRunnerParameter{.gameObject = gameObject, .scriptName = m_className.c_str()}
{
    auto game = Game::getInstance();

    const GameAssemblyInfo &info = game->gameAssemblyInfo();

    auto it = std::ranges::find_if(info.Components,
                                   [this](const ScriptComponentInfo &info) { return info.FullName == m_className; });

    if (it != info.Components.end())
    {
        updateFields(*it);
    }
}

auto ScriptComponent::init() -> void
{
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

#define MAKE_COMPONENT_FIELD(CType, EngineType)                                                                        \
    std::make_unique<TypedComponentField<CType>>(                                                                      \
        propertyInfo.Name, EngineType, [this, propertyInfo] { return getManagedProperty<CType>(propertyInfo.Name); },  \
        [this, propertyInfo](const CType &value) { setManagedProperty(propertyInfo.Name, value); });

auto ScriptComponent::makeFieldForComponentProperty(const ComponentPropertyInfo &propertyInfo)
    -> std::unique_ptr<ComponentField>
{
    if (propertyInfo.Type == "Boolean")
    {
        return MAKE_COMPONENT_FIELD(bool, ComponentField::FieldType::Bool);
    }
    if (propertyInfo.Type == "Int32")
    {
        return MAKE_COMPONENT_FIELD(int, ComponentField::FieldType::Int);
    }
    if (propertyInfo.Type == "Single")
    {
        return MAKE_COMPONENT_FIELD(float, ComponentField::FieldType::Float);
    }
    if (propertyInfo.Type == "Vector2")
    {
        return MAKE_COMPONENT_FIELD(glm::vec2, ComponentField::FieldType::Vec2);
    }
    if (propertyInfo.Type == "Vector3")
    {
        return MAKE_COMPONENT_FIELD(glm::vec3, ComponentField::FieldType::Vec3);
    }
    if (propertyInfo.Type == "Vector4")
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

// TODO: Refactor getManagedProperty methods
template <ComponentFieldDataType T> auto ScriptComponent::getManagedProperty(const std::string &propertyName) -> T
{
    if (!Game::getInstance()->isGameAssemblyLoaded())
    {
        auto &variantValue = m_currentValues[propertyName];

        if (variantValue.index() == 0) // std::monostate
        {
            variantValue = T();
        }

        return std::get<T>(variantValue);
    }

    CSharpExecutionEngine *engine = CSharpExecutionEngine::Get();

    component_entry_point_fn function = engine->getComponentEntryPointFunctions()[CSharpExecutionEngine::GetProperty];

    if (!function)
    {
        throw std::runtime_error("Failed to set property");
    }

    T data;

    GetOrSetPropertyParameters parameter{.gameObject = gameObject(),
                                         .componentName = name().c_str(),
                                         .propertyName = propertyName.c_str(),
                                         .valuePtr = &data};

    if (function(&parameter, sizeof(parameter)))
    {
        throw std::runtime_error(
            std::format("Error setting property {} from script component {}", propertyName, m_className));
    }

    return data;
}

template <ComponentFieldDataType T>
auto ScriptComponent::setManagedProperty(const std::string &propertyName, const T &data) -> void
{
    m_currentValues[propertyName] = data;

    if (!Game::getInstance()->isGameAssemblyLoaded())
    {
        return;
    }

    CSharpExecutionEngine *engine = CSharpExecutionEngine::Get();

    const component_entry_point_fn function =
        engine->getComponentEntryPointFunctions()[CSharpExecutionEngine::SetProperty];

    if (!function)
    {
        throw std::runtime_error("Failed to set property");
    }

    GetOrSetPropertyParameters parameters{.gameObject = gameObject(),
                                          .componentName = name().c_str(),
                                          .propertyName = propertyName.c_str(),
                                          .valuePtr = &data};

    if (function(&parameters, sizeof(parameters)))
    {
        throw std::runtime_error(
            std::format("Error setting property {} from script component {}", propertyName, m_className));
    }
}

} // namespace SSGE

// TODO: Move these to an interop utils file
extern "C"
{
    auto ScriptComponent_SetCurrentValueString(std::string *nativeStr, const char *managedStr) -> void
    {
        *nativeStr = managedStr;
    }

    auto ScriptComponent_GetCurrentValueString(const std::string *nativeStr) -> const char *
    {
        // ReSharper disable once CppDFALocalValueEscapesFunction
        return nativeStr->c_str();
    }
}
