#include "RectangleItem.h"


RectangleItem::RectangleItem(glm::vec2 topLeft, float width, float height, glm::vec3 fillColor) :
	RendererItem(RendererItemType::Rectangle),
	m_topLeft(),
	m_width(),
	m_height(),
    m_fillColor(fillColor)
{
	setRectangle(topLeft, width, height);
}

void RectangleItem::updateGeometry() {
    glm::vec2 normalizedTopLeft = { -1.0f, -1.0f };
    float normalizedWidth = m_width;
    float normalizedHeight = m_height;
    std::vector<Vertex> vertices = {
        {normalizedTopLeft, m_fillColor},
        {glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y), m_fillColor},
        {glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y + normalizedHeight), m_fillColor },
        {glm::vec2(normalizedTopLeft.x, normalizedTopLeft.y + normalizedHeight), m_fillColor}
    };   
    
    geometryUpdated(vertices.data(), sizeof(Vertex) * vertices.size(), {0, 1, 2, 2, 3, 0});
}

glm::vec3 RectangleItem::getFillColor() const {
    return m_fillColor;
}

void RectangleItem::setFillColor(const glm::vec3& fillColor) {
    m_fillColor = fillColor;
}
