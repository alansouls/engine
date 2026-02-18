#pragma once

#include <cstdlib>
#include <string>
#include <vector>

namespace SSGE
{

struct C_ComponentPropertyInfo
{
    wchar_t *Name;
    wchar_t *Type;
};

struct C_ScriptComponentInfo
{
    wchar_t *Name;
    wchar_t *FullName;
    C_ComponentPropertyInfo **Properties;
    int32_t PropertiesLength;
};

struct C_GameAssemblyInfo
{
    wchar_t *Name;
    C_ScriptComponentInfo **Components;
    int32_t ComponentsLength;
};

struct ComponentPropertyInfo
{
    std::wstring Name;
    std::wstring Type;

    static auto FromC_ComponentPropertyInfo(const C_ComponentPropertyInfo *c_info) -> ComponentPropertyInfo;
};

struct ScriptComponentInfo
{
    std::wstring Name;
    std::wstring FullName;
    std::vector<ComponentPropertyInfo> Properties;

    static auto FromC_ScriptComponentInfo(const C_ScriptComponentInfo *c_info) -> ScriptComponentInfo;
};

struct GameAssemblyInfo
{
    std::wstring Name;
    std::vector<ScriptComponentInfo> Components;

    static auto FromC_GameAssemblyName(const C_GameAssemblyInfo *c_info) -> GameAssemblyInfo;
};

} // namespace SSGE

extern "C"
{
    auto GameAssemblyInfo_FromC_GameAssemblyInfo(const SSGE::C_GameAssemblyInfo *c_info, SSGE::GameAssemblyInfo *info)
        -> void;
}