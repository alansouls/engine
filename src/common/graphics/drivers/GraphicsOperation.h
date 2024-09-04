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

	Type type;
	uint32_t key;
	std::optional<std::vector<Vertex>> vertices;
	std::optional<std::vector<uint16_t>> indices;
	std::optional<glm::vec3> transformPosition;
	std::optional<glm::vec3> transformScale;

	std::optional<uint32_t> result;

	void (*callback)();
};