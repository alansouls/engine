#include <optional>
#include <vector>
#include <glm/glm.hpp>
#include <array>
#include "../utils/Vertex.h"

struct GraphicsOperation {
	enum Type {
		Add,
		Remove,
		Update
	};

	enum ElementType {
		Quad,
		Circle
	};

	Type type;
	uint32_t key;
	void* vertexData;
	size_t vertexDataSize;
	std::optional<std::vector<uint16_t>> indices;
	std::optional<glm::vec3> transformPosition;
	std::optional<glm::vec3> transformScale;
	std::optional<ElementType> elementType;

	std::optional<uint32_t> result;

	void (*callback)();
};