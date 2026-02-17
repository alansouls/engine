#pragma once
#include <cstdlib>
#include <string>
#include <vector>

namespace SSGE
{

struct C_ComponentPropertyInfo
{
    char *Name;
    char *Type;

    ~C_ComponentPropertyInfo()
    {
        C_ComponentPropertyInfo::destroyC_ComponentPropertyInfo(this);
    }

    static auto destroyC_ComponentPropertyInfo(C_ComponentPropertyInfo *componentPropertyInfo) -> void
    {
        std::free(componentPropertyInfo->Name);
        std::free(componentPropertyInfo->Type);
    }
};

struct C_ScriptComponentInfo
{
    char *Name;
    char *FullName;
    C_ComponentPropertyInfo **Properties;
    size_t PropertiesLength;

    ~C_ScriptComponentInfo()
    {
        C_ScriptComponentInfo::destroyC_ScriptComponentInfo(this);
    }

    static auto destroyC_ScriptComponentInfo(C_ScriptComponentInfo *scriptComponentInfo) -> void
    {
        std::free(scriptComponentInfo->Name);
        std::free(scriptComponentInfo->FullName);
        for (size_t i = 0; i < scriptComponentInfo->PropertiesLength; ++i)
        {
            delete scriptComponentInfo->Properties[i];
            C_ComponentPropertyInfo::destroyC_ComponentPropertyInfo(scriptComponentInfo->Properties[i]);
        }
        std::free(scriptComponentInfo->Properties);
    }
};

struct C_GameAssemblyInfo
{
    char *Name;
    C_ScriptComponentInfo **Components;
    size_t ComponentsLength;

    ~C_GameAssemblyInfo()
    {
        C_GameAssemblyInfo::destroyC_GameAssemblyInfo(this);
    }

    static auto destroyC_GameAssemblyInfo(C_GameAssemblyInfo *gameAssemblyInfo) -> void
    {
        std::free(gameAssemblyInfo->Name);
        for (size_t i = 0; i < gameAssemblyInfo->ComponentsLength; ++i)
        {
            delete gameAssemblyInfo->Components[i];
        }
        std::free(gameAssemblyInfo->Components);
    }
};

struct ComponentPropertyInfo
{
    std::string Name;
    std::string Type;

    static auto FromC_ComponentPropertyInfo(const C_ComponentPropertyInfo *c_info) -> ComponentPropertyInfo
    {
        return {.Name = c_info->Name, .Type = c_info->Type};
    }
};

struct ScriptComponentInfo
{
    std::string Name;
    std::string FullName;
    std::vector<ComponentPropertyInfo> Properties;

    static auto FromC_ScriptComponentInfo(const C_ScriptComponentInfo *c_info) -> ScriptComponentInfo
    {
        std::vector<ComponentPropertyInfo> properties(c_info->PropertiesLength);
        for (size_t i = 0; i < c_info->PropertiesLength; ++i)
        {
            properties[i] = ComponentPropertyInfo::FromC_ComponentPropertyInfo(c_info->Properties[i]);
        };
        return {.Name = c_info->Name, .FullName = c_info->FullName, .Properties = properties};
    }
};

struct GameAssemblyInfo
{
    std::string Name;
    std::vector<ScriptComponentInfo> Components;

    static auto FromC_GameAssemblyName(const C_GameAssemblyInfo *c_info) -> GameAssemblyInfo
    {
        std::vector<ScriptComponentInfo> components(c_info->ComponentsLength);
        for (size_t i = 0; i < c_info->ComponentsLength; ++i)
        {
            components[i] = ScriptComponentInfo::FromC_ScriptComponentInfo(c_info->Components[i]);
        }
        return {.Name = std::string(c_info->Name), .Components = std::move(components)};
    }
};

} // namespace SSGE
