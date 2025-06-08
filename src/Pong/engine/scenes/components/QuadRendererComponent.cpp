#include "QuadRendererComponent.h"

namespace SSGE
{

const std::string QuadRendererComponent::TypeName = "QuadRendererComponent";

QuadRendererComponent::QuadRendererComponent(GameObject *gameObject)
    : RendererComponent(gameObject, createItem().get(), TypeName), m_topLeft(1.0f, 1.0f), m_width(100.0f), m_height(100.0f),
      m_fillColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}

auto QuadRendererComponent::setTopLeft(const glm::vec2 topLeft) -> void
{
    m_topLeft = topLeft;
}

auto QuadRendererComponent::setWidth(float width) -> void
{
    m_width = width;
}

auto QuadRendererComponent::setHeight(float height) -> void
{
    m_height = height;
}

auto QuadRendererComponent::setFillColor(glm::vec4 fillColor) -> void
{
    m_fillColor = fillColor;
}

auto QuadRendererComponent::createItem() -> std::unique_ptr<RectangleItem> &
{
    m_topLeft = glm::vec2(1.0f, 1.0f);
    m_width = 100.0f;
    m_height = 100.0f;
    m_fillColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_item = std::make_unique<RectangleItem>(m_topLeft, m_width, m_height, m_fillColor);
    return m_item;
}

} // namespace SSGE