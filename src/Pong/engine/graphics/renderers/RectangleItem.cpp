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
        {normalizedTopLeft},
        {glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y)},
        {glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y + normalizedHeight)},
        {glm::vec2(normalizedTopLeft.x, normalizedTopLeft.y + normalizedHeight)}
    };   
    
    geometryUpdated(vertices.data(), sizeof(Vertex) * vertices.size(), {0, 1, 2, 2, 3, 0});
}

glm::vec3 RectangleItem::getFillColor() const {
    return m_fillColor;
}

void RectangleItem::setFillColor(const glm::vec3& fillColor) {
    m_fillColor = fillColor;
}
