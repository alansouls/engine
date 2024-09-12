#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "Renderer.h"
#include "../utils/Vertex.h"

struct TransformUpdatedCallback
{
    void* caller;
    void (*callback) (void*, uint32_t);

    bool isSameAs(const TransformUpdatedCallback& other)
    {
        return caller == other.caller && callback == other.callback;
    }
};

class RendererItem
{
public:
	enum RendererItemType {
		Triangle,
		Rectangle,
		Circle,
		RendererItemTypeCount
	};
    
	virtual ~RendererItem() {
		if (m_vertexData != nullptr) {
			delete m_vertexData;
		}
	}

	virtual void updateGeometry() = 0;

	virtual void updateTransform() = 0;

	void* getVertexData() const {
		return m_vertexData;
	}

	size_t getVertexDataSize() const {
		return m_vertexDataSize;
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
        executeTransformUpdatedCallbacks();
	}
    
    void setPosition(glm::vec3 position){
        setTransform(position, m_transformScale);
    }

	void setScale(glm::vec3 scale) {
        setTransform(m_transformPosition, scale);
	}

	uint32_t getKey() const {
		return m_key;
	}

	void setKey(uint32_t key) {
		m_key = key;
	}
    
    void addCallback(void* callerPtr, void (*callback) (void*, uint32_t)) {
        m_transformUpdatedCallbacks.push_back({callerPtr, callback});
    }
    
    void removeCallback(void* callerPtr, void (*callback) (void*, uint32_t)) {
        TransformUpdatedCallback toRemove = {callerPtr, callback};
        auto iter = m_transformUpdatedCallbacks.begin();
        while (iter != m_transformUpdatedCallbacks.end()) {
            if ((*iter).isSameAs(toRemove)) {
                m_transformUpdatedCallbacks.erase(iter);
                return;
            }
            iter++;
        }
    }

	RendererItemType getType() const {
		return m_type;
	}

protected:
	RendererItem(RendererItemType type) :
		m_type(type),
		m_transformPosition(glm::vec3(0.0f)),
		m_key(0),
		m_transformScale(glm::vec3(1.0f)),
		m_vertexData(nullptr),
		m_vertexDataSize(0)
	{
	}

	void geometryUpdated(void* vertexData, size_t vertexDataSize, const std::vector<uint16_t> &indices) {
		if (m_vertexData != nullptr) {
			delete m_vertexData;
		}
		if (vertexData == nullptr) {
			m_vertexData = nullptr;
			m_vertexDataSize = 0;
			return;
		}
		m_vertexData = malloc(vertexDataSize);
		memcpy(m_vertexData, vertexData, vertexDataSize);
		m_vertexDataSize = vertexDataSize;
		m_indices = indices;
	}
    
private:
	uint32_t m_key;
	RendererItemType m_type;

	std::vector<uint16_t> m_indices;
	glm::vec3 m_transformPosition;
	glm::vec3 m_transformScale;

	void* m_vertexData = nullptr;
	size_t m_vertexDataSize;
    
    std::vector<TransformUpdatedCallback> m_transformUpdatedCallbacks;
    
    void executeTransformUpdatedCallbacks() {
        for (auto callback : m_transformUpdatedCallbacks) {
            callback.callback(callback.caller, m_key);
        }
    }
};

