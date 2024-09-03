#pragma once
#include "RendererItem.h"
#include "Renderer.h"

class RectangleItem :
    public RendererItem
{
public:
	RectangleItem(glm::vec2 topLeft, float width, float height, glm::vec3 fillColor, Renderer::CoordinatesType coordinatesType, Renderer* renderer);
	~RectangleItem() {}

	void setRectangle(glm::vec2 topLeft, float width, float height) {
		m_topLeft = topLeft;
		m_width = width;
		m_height = height;

		geometryUpdated(m_renderer->getRectangleVertices(topLeft, width, height, m_coordinatesType, getFillColor()), { 0, 1, 2, 2, 3, 0 });
	}

	void updateGeometry() override {
		geometryUpdated(m_renderer->getRectangleVertices(m_topLeft, m_width, m_height, m_coordinatesType, getFillColor()), { 0, 1, 2, 2, 3, 0 });
	}

	glm::vec2 getTopLeft() const {
		return m_topLeft;
	}

	float getWidth() const {
		return m_width;
	}

	float getHeight() const {
		return m_height;
	}

private:
	glm::vec2 m_topLeft;
	float m_width;
	float m_height;
	Renderer::CoordinatesType m_coordinatesType;
};

