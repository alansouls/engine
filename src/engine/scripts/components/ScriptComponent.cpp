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

} // namespace SSGE