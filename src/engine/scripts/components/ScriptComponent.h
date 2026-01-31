#pragma once
#include "engine/scenes/Component.h"
#include <filesystem>
#include <vector>

namespace SSGE
{

struct ScriptRunnerParameter
{
    GameObject* gameObject;
    const char* scriptName;
};

struct SetPropertiesParameter
{
    GameObject* gameObject;
    const char* scriptName;
    const char* propertyName;
    const char* propertyValue;
};

class ScriptComponent : public Component
{
  public:
    ScriptComponent(GameObject *gameObject, std::string className);

    auto init() -> void override;

    auto update() -> void override;

    auto setProperty(const std::string &propertyName, const std::string &propertyValue) -> void;

  private:
    std::string m_className;
    ScriptRunnerParameter m_scriptRunnerParameter;
    std::vector<std::pair<std::string, std::string>> m_pendingProperties;

    auto commitProperties() -> void;
    auto setPropertyManaged(const std::string &propertyName, const std::string &propertyValue) const -> void;
};
} // namespace SSGE