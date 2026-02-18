#include "GameAssemblyInfo.h"

#if defined(WINDOWS)
#define CHAR_PTR__TO_STRING(S) wstringToString(std::wstring(S))
#else
#define CHAR_PTR__TO_STRING(S) std::string(S)
#endif

namespace SSGE
{

inline static auto wstringToString(const std::wstring &wstr) -> std::string
{
    std::string str(wstr.begin(), wstr.end());
    return str;
}

auto GameAssemblyInfo::FromC_GameAssemblyName(const C_GameAssemblyInfo *c_info) -> GameAssemblyInfo
{
    std::vector<ScriptComponentInfo> components(c_info->ComponentsLength);
    for (int32_t i = 0; i < c_info->ComponentsLength; ++i)
    {
        components[i] = ScriptComponentInfo::FromC_ScriptComponentInfo(c_info->Components[i]);
    }
    return {.Name = CHAR_PTR__TO_STRING(c_info->Name), .Components = std::move(components)};
}

auto ScriptComponentInfo::FromC_ScriptComponentInfo(const C_ScriptComponentInfo *c_info) -> ScriptComponentInfo
{
    std::vector<ComponentPropertyInfo> properties(c_info->PropertiesLength);
    for (int32_t i = 0; i < c_info->PropertiesLength; ++i)
    {
        properties[i] = ComponentPropertyInfo::FromC_ComponentPropertyInfo(c_info->Properties[i]);
    };
    return {.Name = CHAR_PTR__TO_STRING(c_info->Name),
            .FullName = CHAR_PTR__TO_STRING(c_info->FullName),
            .Properties = properties};
}

auto ComponentPropertyInfo::FromC_ComponentPropertyInfo(const C_ComponentPropertyInfo *c_info) -> ComponentPropertyInfo
{
    return {.Name = CHAR_PTR__TO_STRING(c_info->Name), .Type = CHAR_PTR__TO_STRING(c_info->Type)};
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