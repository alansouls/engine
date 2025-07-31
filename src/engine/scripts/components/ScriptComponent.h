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
    ScriptComponent(GameObject *gameObject, std::string scriptName);

    auto init() -> void override;

    auto update() -> void override;

  private:
    std::string m_scriptName;
    ScriptRunnerParameter m_scriptRunnerParameter;

    static constexpr std::string_view ExecuteComponentClassName = "SSGEDotNet.Core.Scene.ScriptRunner";
    static constexpr std::string_view ExecuteComponentInitMethodName = "CallComponentInit";
    static constexpr std::string_view ExecuteComponentUpdateMethodName = "CallComponentUpdate";
};
} // namespace SSGE