#include "CircleItem.h"

CircleItem::CircleItem(glm::vec2 center, float radius, glm::vec3 fillColor) : RendererItem(RendererItemType::Circle),
    m_center(center), 
    m_radius(radius), 
    m_fillColor(fillColor)
{
	setCircle(center, radius);
}

glm::vec3 CircleItem::getFillColor() const
{
	return m_fillColor;
}

void CircleItem::setFillColor(const glm::vec3& fillColor)
{
	m_fillColor = fillColor;
}
