#pragma once

#include "ScriptExecutionEngine.h"

namespace SSGE
{
class CSharpExecutionEngine : public ScriptExecutionEngine
{
  public:
    CSharpExecutionEngine(std::filesystem::path dotnetProjectPath);
    ~CSharpExecutionEngine() override = default;

    auto init() -> void override;

    auto compile() -> bool override;

    auto execute() -> void override;

private:
    std::filesystem::path m_dotnetProjectPath;
};

static std::unique_ptr<CSharpExecutionEngine> Instance;

} // namespace SSGE
