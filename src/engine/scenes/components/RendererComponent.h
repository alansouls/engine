#pragma once

#include "engine/graphics/renderers/scene/RendererItem.h"
#include "engine/scenes/Component.h"
#include <memory>

namespace SSGE
{
    class RendererComponent : public Component
    {
    public:
        RendererComponent(GameObject* gameObject, std::unique_ptr<RendererItem> item, const std::string& typeName);
        ~RendererComponent() override = 0;
        auto init() -> void override;
        auto update() -> void override;

    protected:
        bool m_changed;
        std::unique_ptr<RendererItem> m_item;

    private:
        GameObject* m_gameObject;
    };
} // namespace SSGE
