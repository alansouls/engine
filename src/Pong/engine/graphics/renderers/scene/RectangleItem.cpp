#include "RectangleItem.h"

RectangleItem::RectangleItem(glm::vec2 topLeft, float width, float height, glm::vec3 fillColor)
    : RendererItem(RendererItemType::Rectangle), m_topLeft(), m_width(), m_height(), m_fillColor(fillColor)
{
    setRectangle(topLeft, width, height);
}

glm::vec3 RectangleItem::getFillColor() const
{
    return m_fillColor;
}

void RectangleItem::setFillColor(const glm::vec3 &fillColor)
{
    m_fillColor = fillColor;
}
