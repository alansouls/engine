#include "CircleRendererComponent.h"

namespace SSGE
{

const std::string CircleRendererComponent::TypeName = "CircleRendererComponent";

CircleRendererComponent::CircleRendererComponent(GameObject *gameObject)
    : RendererComponent(gameObject, createItem(), TypeName), m_center(0.0f, 0.0f), m_radius(1.0f),
      m_fillColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}

auto CircleRendererComponent::setCenter(const glm::vec2 &center) -> void
{
    m_center = center;
    auto item = dynamic_cast<CircleItem *>(m_item.get());
    item->setCircle(m_center, m_radius);
}

auto CircleRendererComponent::setRadius(float radius) -> void
{
    m_radius = radius;
    auto item = dynamic_cast<CircleItem *>(m_item.get());
    item->setCircle(m_center, m_radius);
}

auto CircleRendererComponent::setFillColor(const glm::vec4 &fillColor) -> void
{
    m_fillColor = fillColor;
    auto item = dynamic_cast<CircleItem *>(m_item.get());
    item->setFillColor(m_fillColor);
}

auto CircleRendererComponent::createItem() -> std::unique_ptr<CircleItem>
{
    return std::make_unique<CircleItem>(m_center, m_radius, m_fillColor);
}
} // namespace SSGE