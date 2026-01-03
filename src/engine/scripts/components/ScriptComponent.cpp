#include "ScriptComponent.h"
#include "scenes/GameObject.h"
#include "scripts/CSharpExecutionEngine.h"

#include <iostream>
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
    auto engine = CSharpExecutionEngine::Get();

    component_entry_point_fn initFunction = engine->getComponentEntryPointFunctions()[0];

    if (initFunction == nullptr)
    {
        return;
    }

    int a ;
    std::cin >> a;
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

auto ScriptComponent::setProperty(const std::string &propertyName, const std::string &propertyValue) -> void
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