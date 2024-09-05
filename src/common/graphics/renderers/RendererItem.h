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

	virtual void updateGeometry() = 0;

	virtual void updateTransform() = 0;

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

	glm::vec3 getTransformPosition() const {
		return m_transformPosition;
	}

	glm::vec3 getTransformScale() const {
		return m_transformScale;
	}

	void setTransform(glm::vec3 position, glm::vec3 scale) {
		m_transformPosition = position;
		m_transformScale = scale;
        m_renderer->m_updatedSet.insert(m_key);
	}

	uint32_t getKey() const {
		return m_key;
	}

	void setKey(uint32_t key) {
		m_key = key;
	}


protected:
	RendererItem(RendererItemType type, const glm::vec3 &fillColor, Renderer* renderer) : 
		m_fillColor(fillColor),
		m_type(type),
		m_renderer(renderer),
		m_transformPosition(glm::vec3(0.0f)),
		m_key(0)
	{
	}

	void geometryUpdated(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices) {
		m_vertices = vertices;
		m_indices = indices;
	}

	//TODO: add rotation and scale
	void transformUpdated(glm::vec3 position) {
		m_transformPosition = position;
	}

	Renderer* m_renderer;
private:
	uint32_t m_key;
	glm::vec3 m_fillColor;
	RendererItemType m_type;

	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;
	glm::vec3 m_transformPosition;
	glm::vec3 m_transformScale;
};

inline RendererItem::~RendererItem() {}

