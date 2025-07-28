#pragma once
#include "components/ScriptComponent.h"

#include <unordered_map>

namespace SSGE
{
class ScriptExecutionEngine
{
  public:
    virtual ~ScriptExecutionEngine() = default;

    virtual auto compile() -> bool = 0;

    virtual auto execute(const std::string &entryPointClass, const std::string &entryPointMethod) -> void = 0;

    static auto CreateExecutionEngine(ScriptComponent::ScriptType type) -> ScriptExecutionEngine*;

  private:

    virtual auto init() -> void = 0;

    static std::unordered_map<ScriptComponent::ScriptType, std::unique_ptr<ScriptExecutionEngine>> s_engines;
};
} // namespace SSGE
