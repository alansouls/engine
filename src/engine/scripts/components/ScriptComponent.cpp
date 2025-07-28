#include "ScriptComponent.h"
#include "engine/scripts/ScriptExecutionEngine.h"

#include <utility>

namespace SSGE
{
ScriptComponent::ScriptComponent(GameObject *gameObject, ScriptType type, std::filesystem::path filePath)
    : Component("ScriptComponent", gameObject), m_type(type), m_filePath(std::move(filePath))
{
}

auto ScriptComponent::init() -> void
{
    ScriptExecutionEngine *engine = nullptr;
    switch (m_type)
    {
    case CSharp:
        engine = ScriptExecutionEngine::CreateExecutionEngine(CSharp);
        break;
    }

    //TODO
}

auto ScriptComponent::update() -> void
{
}

} // namespace SSGE