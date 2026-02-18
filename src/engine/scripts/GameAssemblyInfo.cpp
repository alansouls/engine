#include "GameAssemblyInfo.h"

namespace SSGE
{

auto GameAssemblyInfo::FromC_GameAssemblyName(const C_GameAssemblyInfo *c_info) -> GameAssemblyInfo
{
    std::vector<ScriptComponentInfo> components(c_info->ComponentsLength);
    for (int32_t i = 0; i < c_info->ComponentsLength; ++i)
    {
        components[i] = ScriptComponentInfo::FromC_ScriptComponentInfo(c_info->Components[i]);
    }
    return {.Name = std::wstring(c_info->Name), .Components = std::move(components)};
}

auto ScriptComponentInfo::FromC_ScriptComponentInfo(const C_ScriptComponentInfo *c_info) -> ScriptComponentInfo
{
    std::vector<ComponentPropertyInfo> properties(c_info->PropertiesLength);
    for (int32_t i = 0; i < c_info->PropertiesLength; ++i)
    {
        properties[i] = ComponentPropertyInfo::FromC_ComponentPropertyInfo(c_info->Properties[i]);
    };
    return {.Name = std::wstring(c_info->Name), .FullName = std::wstring(c_info->FullName), .Properties = properties};
}

auto ComponentPropertyInfo::FromC_ComponentPropertyInfo(const C_ComponentPropertyInfo *c_info) -> ComponentPropertyInfo
{
    return {.Name = std::wstring(c_info->Name), .Type = std::wstring(c_info->Type)};
}

} // namespace SSGE

extern "C"
{
    auto GameAssemblyInfo_FromC_GameAssemblyInfo(const SSGE::C_GameAssemblyInfo *c_info, SSGE::GameAssemblyInfo *info)
        -> void
    {
        *info = SSGE::GameAssemblyInfo::FromC_GameAssemblyName(c_info);
    }
}