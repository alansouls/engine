#pragma once

#include "engine/graphics/renderers/scene/RendererItem.h"
#include "engine/scenes/Component.h"

#include <glm/mat4x4.hpp>
#include <memory>

namespace SSGE
{
class RendererComponent : public Component
{
  public:
    RendererComponent(GameObject *gameObject, std::unique_ptr<RendererItem> item, std::string name,
                      std::string displayName, Component::ComponentType componenType);
    ~RendererComponent() override = default;
    auto init() -> void override;
    auto update() -> void override;

  protected:
    bool m_changed;
    std::unique_ptr<RendererItem> m_item;

  private:
    GameObject *m_gameObject;
    glm::mat4 m_latestWorldTransform;
};
} // namespace SSGE
