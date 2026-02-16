#pragma once
#include <string>
#include <vector>

namespace SSGE
{
struct ComponentPropertyInfo
{
    std::string name;
    std::string type;
};

struct ScriptComponentInfo
{
    std::string Name;
    std::string FullName;
    std::vector<ComponentPropertyInfo> Properties;
};

struct GameAssemblyInfo
{
    std::string Name;
    std::vector<ScriptComponentInfo> Components;
};
} // namespace SSGE