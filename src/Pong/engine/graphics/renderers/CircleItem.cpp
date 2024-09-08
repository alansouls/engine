#include "CircleItem.h"

CircleItem::CircleItem(glm::vec2 center, float radius, glm::vec3 fillColor) : RendererItem(RendererItemType::Circle),
    m_center(center), 
    m_radius(radius), 
    m_fillColor(fillColor)
{
	setCircle(center, radius);
}

void CircleItem::updateGeometry()
{
    glm::vec2 normalizedTopLeft = { 0.0f - m_radius, 0.0f - m_radius };
    float normalizedWidth = m_radius * 2.0f;
    float normalizedHeight = m_radius * 2.0f;
    std::vector<CircleVertex> vertices = {
        {normalizedTopLeft, m_fillColor, m_radius},
        {glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y), m_fillColor, m_radius},
        {glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y + normalizedHeight), m_fillColor, m_radius },
        {glm::vec2(normalizedTopLeft.x, normalizedTopLeft.y + normalizedHeight), m_fillColor, m_radius}
    };

    geometryUpdated(vertices.data(), sizeof(CircleVertex) * vertices.size(), {0, 1, 2, 2, 3, 0});
}

glm::vec3 CircleItem::getFillColor() const
{
	return m_fillColor;
}

void CircleItem::setFillColor(const glm::vec3& fillColor)
{
	m_fillColor = fillColor;
}
