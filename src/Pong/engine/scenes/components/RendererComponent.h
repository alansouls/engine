#pragma once
#include "engine/graphics/renderers/scene/RendererItem.h"
#include "engine/scenes/Component.h"

namespace SSGE
{
class RendererComponent : public Component
{
  public:
    RendererComponent(GameObject* gameObject, RendererItem *item, const std::string &typeName);
    ~RendererComponent() override = default;
    auto init() -> void override;
    auto update() -> void override;
protected:
    bool m_changed;

private:
    GameObject* m_gameObject;
    RendererItem* m_item;
};
}
