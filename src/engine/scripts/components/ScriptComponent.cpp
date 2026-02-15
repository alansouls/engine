#include "ScriptComponent.h"
#include "scenes/GameObject.h"
#include "scripts/CSharpExecutionEngine.h"

#include <utility>

namespace SSGE
{
ScriptComponent::ScriptComponent(GameObject *gameObject, std::string className)
    : Component("ScriptComponent", gameObject), m_className(std::move(className)),
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

auto ScriptComponent::setProperty(const std::string &propertyName, const std::string &propertyValue) -> void
{
    m_pendingProperties.emplace_back(propertyName, propertyValue);
}

auto ScriptComponent::commitProperties() -> void
{
    // TODO optimize this passing all properties at once
    for (auto &[name, value] : m_pendingProperties)
    {
        setPropertyManaged(name, value);
    }

    m_pendingProperties.clear();
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