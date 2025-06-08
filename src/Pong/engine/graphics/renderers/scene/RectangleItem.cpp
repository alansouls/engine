#include "RectangleItem.h"

RectangleItem::RectangleItem(glm::vec2 topLeft, float width, float height, glm::vec4 fillColor)
    : RendererItem(Rectangle), m_topLeft(), m_width(), m_height(), m_fillColor(fillColor)
{
    setRectangle(topLeft, width, height);
}

auto RectangleItem::getTopLeft() const -> glm::vec2
{
    return getTransformPosition();
}

auto RectangleItem::moveY(float step) -> void
{
    setRectangle({m_topLeft.x, m_topLeft.y + step}, m_width, m_height);
}

auto RectangleItem::moveXTo(float newX) -> void
{
    setRectangle({newX, m_topLeft.y}, m_width, m_height);
}

auto RectangleItem::getWidth() const -> float
{
    return m_width;
}

auto RectangleItem::setHeight(float height) -> void
{
    m_height = height;
}

auto RectangleItem::setWidth(float width) -> void
{
    m_width = width;
}

auto RectangleItem::getHeight() const -> float
{
    return m_height;
}

auto RectangleItem::getFillColor() const -> glm::vec4
{
    return m_fillColor;
}

auto RectangleItem::setFillColor(const glm::vec4 &fillColor) -> void
{
    m_fillColor = fillColor;
}
