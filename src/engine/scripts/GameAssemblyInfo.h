#pragma once

#include "coreclr_delegates.h"
#include <cstdlib>
#include <string>
#include <vector>

namespace SSGE
{

struct C_ComponentPropertyInfo
{
    char_t *Name;
    char_t *Type;
};

struct C_ScriptComponentInfo
{
    char_t *Name;
    char_t *FullName;
    C_ComponentPropertyInfo **Properties;
    int32_t PropertiesLength;
};

struct C_GameAssemblyInfo
{
    char_t *Name;
    C_ScriptComponentInfo **Components;
    int32_t ComponentsLength;
};

struct ComponentPropertyInfo
{
    std::string Name;
    std::string Type;

    static auto FromC_ComponentPropertyInfo(const C_ComponentPropertyInfo *c_info) -> ComponentPropertyInfo;
};

struct ScriptComponentInfo
{
    std::string Name;
    std::string FullName;
    std::vector<ComponentPropertyInfo> Properties;

    static auto FromC_ScriptComponentInfo(const C_ScriptComponentInfo *c_info) -> ScriptComponentInfo;
};

struct GameAssemblyInfo
{
    std::string Name;
    std::vector<ScriptComponentInfo> Components;

    static auto FromC_GameAssemblyName(const C_GameAssemblyInfo *c_info) -> GameAssemblyInfo;
};

} // namespace SSGE

extern "C"
{
    auto GameAssemblyInfo_FromC_GameAssemblyInfo(const SSGE::C_GameAssemblyInfo *c_info, SSGE::GameAssemblyInfo *info)
        -> void;
}