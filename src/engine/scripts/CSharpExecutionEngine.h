#pragma once

#include "ScriptExecutionEngine.h"
#include "coreclr_delegates.h"

#include <map>

namespace SSGE
{
class CSharpExecutionEngine : public ScriptExecutionEngine
{
  public:
    CSharpExecutionEngine(std::string projectName, std::filesystem::path dotnetProjectPath);
    ~CSharpExecutionEngine() override = default;

    auto init() -> void override;

    auto compile() -> bool override;

    auto execute(const std::string &entryPointClass, const std::string &entryPointMethod) -> void override;

  private:
    std::string m_projectName;
    std::filesystem::path m_dotnetProjectPath;
    load_assembly_and_get_function_pointer_fn m_loadAndGetFunctionPointer;
    std::map<std::string, component_entry_point_fn> m_componentEntryPoints;
};

static std::unique_ptr<CSharpExecutionEngine> Instance;

} // namespace SSGE
