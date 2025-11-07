#pragma once
#include "engine/scenes/Component.h"
#include <filesystem>

namespace SSGE
{

struct ScriptRunnerParameter
{
    GameObject* gameObject;
    const char* scriptName;
};

class ScriptComponent : public Component
{
  public:
    ScriptComponent(GameObject *gameObject, std::string className);

    auto init() -> void override;

    auto update() -> void override;

  private:
    std::string m_className;
    ScriptRunnerParameter m_scriptRunnerParameter;
};
} // namespace SSGE