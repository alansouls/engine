#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "Renderer.h"
#include "../utils/Vertex.h"

class RendererItem
{
public:
	enum RendererItemType {
		Triangle,
		Rectangle,
		Circle,
		RendererItemTypeCount
	};
    
    virtual ~RendererItem() = 0;

	glm::vec3 getFillColor() const {
		return m_fillColor;
	}

	void setFillColor(const glm::vec3& fillColor) {
		m_fillColor = fillColor;
	}

	std::vector<Vertex> getVertices() const {
		return m_vertices;
	}

	std::vector<uint16_t> getIndices() const {
		return m_indices;
	}

protected:
	RendererItem(RendererItemType type, const glm::vec3 &fillColor, Renderer* renderer) : 
		m_fillColor(fillColor),
		m_type(type),
		m_renderer(renderer)
	{
	}

	void geometryUpdated(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices) {
		m_vertices = vertices;
		m_indices = indices;
		m_renderer->m_changeQueue.push_back(this);
	}

	Renderer* m_renderer;
private:
	glm::vec3 m_fillColor;
	RendererItemType m_type;

	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;
};

inline RendererItem::~RendererItem() {}

