#pragma once
#include "engine/scenes/Component.h"
#include "scripts/GameAssemblyInfo.h"

#include <filesystem>
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
    ScriptComponent(GameObject *gameObject, std::string className);

    auto init() -> void override;

    auto update() -> void override;

    auto setProperty(const std::string &propertyName, const std::string &propertyValue) -> void;

    auto updateFields(const ScriptComponentInfo &info) -> void;

private:
    std::string m_className;
    ScriptRunnerParameter m_scriptRunnerParameter;
    std::vector<std::pair<std::string, std::string>> m_pendingProperties;

    auto commitProperties() -> void;
    auto setPropertyManaged(const std::string &propertyName, const std::string &propertyValue) const -> void;
    [[nodiscard]] auto makeFieldForComponentProperty(
        const ComponentPropertyInfo &propertyInfo) const -> std::unique_ptr<ComponentField>;

    template <ComponentFieldDataType T>
    [[nodiscard]] auto makeFieldForComponentProperty(
        const ComponentPropertyInfo &propertyInfo) const -> std::unique_ptr<ComponentField>;
};
} // namespace SSGE