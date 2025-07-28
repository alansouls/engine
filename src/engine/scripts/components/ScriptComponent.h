#pragma once
#include "engine/scenes/Component.h"
#include <filesystem>

namespace SSGE
{
class ScriptComponent : public Component
{
  public:
    enum ScriptType
    {
        CSharp,
        ScriptTypeCount
    };

    ScriptComponent(GameObject *gameObject, ScriptType type, std::filesystem::path filePath);

    auto init() -> void override;

    auto update() -> void override;

  private:
    ScriptType m_type;
    std::filesystem::path m_filePath;
};
} // namespace SSGE