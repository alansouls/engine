#include "ScriptComponent.h"
#include "scenes/GameObject.h"
#include "scripts/CSharpExecutionEngine.h"

#include <utility>

namespace SSGE
{
ScriptComponent::ScriptComponent(GameObject *gameObject, std::string scriptName)
    : Component("ScriptComponent", gameObject), m_scriptName(std::move(scriptName)),
      m_scriptRunnerParameter{.gameObject = gameObject, .scriptName = m_scriptName.c_str()}
{
}

auto ScriptComponent::init() -> void
{
    auto engine = CSharpExecutionEngine::Get();

    engine->execute(ExecuteComponentClassName, ExecuteComponentInitMethodName, &m_scriptRunnerParameter,
                    sizeof(ScriptRunnerParameter));
}

auto ScriptComponent::update() -> void
{
    auto engine = CSharpExecutionEngine::Get();

    engine->execute(ExecuteComponentClassName, ExecuteComponentUpdateMethodName, &m_scriptRunnerParameter,
                    sizeof(ScriptRunnerParameter));
}

} // namespace SSGE