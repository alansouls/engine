#pragma once
#include "EngineAPI.h"
#include "engine/scenes/Component.h"
#include "scripts/GameAssemblyInfo.h"

#include <filesystem>
#include <variant>
#include <vector>

namespace SSGE
{

struct ScriptRunnerParameter
{
    GameObject *gameObject;
    const char *scriptName;
};

struct SetPropertiesParameter
{
    GameObject *gameObject;
    const char *scriptName;
    const char *propertyName;
    const char *propertyValue;
};

class ScriptComponent : public Component
{
  public:
    ScriptComponent(GameObject *gameObject, std::string fullClassName, std::string className);

    auto init() -> void override;

    auto update() -> void override;

    auto updateFields(const ScriptComponentInfo &info) -> void;

    template <ComponentFieldDataType T> [[nodiscard]] auto getManagedProperty(const std::string &propertyName) -> T;

    template <ComponentFieldDataType T> auto setManagedProperty(const std::string &propertyName, const T &data) -> void;

  private:
    std::string m_className;
    ScriptRunnerParameter m_scriptRunnerParameter;
    std::unordered_map<std::string,
                       std::variant<std::monostate, int, float, bool, std::string, glm::vec2, glm::vec3, glm::vec4>>
        m_currentValues;

    auto setPropertyManaged(const std::string &propertyName, const std::string &propertyValue) const -> void;

    [[nodiscard]] auto makeFieldForComponentProperty(const ComponentPropertyInfo &propertyInfo)
        -> std::unique_ptr<ComponentField>;
};
} // namespace SSGE