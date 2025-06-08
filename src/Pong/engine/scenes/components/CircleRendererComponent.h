#pragma once
#include "RendererComponent.h"
#include "engine/graphics/renderers/scene/CircleItem.h"

#include <glm/glm.hpp>

namespace SSGE
{

class CircleRendererComponent final : public RendererComponent
{
public:
    explicit CircleRendererComponent(GameObject *gameObject);

    auto setCenter(const glm::vec2 &center) -> void;
    auto setRadius(float radius) -> void;
    auto setFillColor(const glm::vec4 &fillColor) -> void;

    static const std::string TypeName;

private:
    std::unique_ptr<CircleItem> m_item;
    glm::vec2 m_center;
    float m_radius;
    glm::vec4 m_fillColor;

    auto createItem() -> std::unique_ptr<CircleItem> &;
};

} // namespace SSGE