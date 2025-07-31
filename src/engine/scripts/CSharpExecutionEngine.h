#pragma once
#include "coreclr_delegates.h"

#include <filesystem>
#include <map>
#include <string>

namespace SSGE
{
class GameObject;
typedef void *(CORECLR_DELEGATE_CALLTYPE *InitAndGetComponentUpdateEntryPoint)(GameObject *gameObject);
class ScriptComponent;
class CSharpExecutionEngine
{
  public:
    static constexpr int ErrorGettingEntryPointFunctionPointerCode = -1;
    static auto GetOrInitialize(const std::string &projectName, const std::filesystem::path &dotnetProjectPath)
        -> CSharpExecutionEngine *;
    static auto Get() -> CSharpExecutionEngine *;

    auto init() -> void;

    auto compile() -> bool;

    auto execute(const std::string_view &entryPointClass, const std::string_view &entryPointMethod, void *data,
                 int32_t dataLength) -> int;

  private:
    CSharpExecutionEngine(std::string projectName, std::filesystem::path dotnetProjectPath);
    std::string m_projectName;
    std::filesystem::path m_dotnetProjectPath;
    load_assembly_and_get_function_pointer_fn m_loadAndGetFunctionPointer;
    std::map<std::string, void *> m_componentEntryPoints;

    static std::unique_ptr<CSharpExecutionEngine> s_instance;

    auto getEntryPointFunctionPointer(const std::string_view &entryPointClass, const std::string_view &entryPointMethod)
        -> void *;
};

} // namespace SSGE
