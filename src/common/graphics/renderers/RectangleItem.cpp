#include "RectangleItem.h"


RectangleItem::RectangleItem(glm::vec2 topLeft, float width, float height, glm::vec3 fillColor, Renderer::CoordinatesType coordinatesType, Renderer* renderer) :
	RendererItem(RendererItemType::Rectangle, fillColor, renderer), 
	m_topLeft(),
	m_width(),
	m_height(),
	m_coordinatesType(coordinatesType)
{
	setRectangle(topLeft, width, height);
}