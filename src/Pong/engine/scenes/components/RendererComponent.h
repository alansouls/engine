#pragma once
#include "engine/graphics/renderers/scene/RendererItem.h"
#include "engine/scenes/Component.h"

namespace SSGE
{
class RendererComponent : public Component
{
  public:
    RendererComponent(GameObject* gameObject, RendererItem *item);
    ~RendererComponent() override = 0;
    auto init() -> void override;
    auto update() -> void override;
    auto gameObject() -> GameObject & override;

     static const std::string TypeName;
protected:
    bool m_changed;

private:
    GameObject* m_gameObject;
    RendererItem* m_item;
};
}
