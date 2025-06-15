#include "CircleItem.h"

CircleItem::CircleItem(glm::vec2 center, float radius, const glm::vec4 &fillColor) : RendererItem(RendererItemType::Circle),
    m_center(center), 
    m_radius(radius)
{
	setCircle(center, radius);
    setFillColor(fillColor);
}
