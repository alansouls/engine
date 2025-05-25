#include "GraphicsDriver.h"
#include <glm/glm.hpp>
#include <optional>
#include <vector>

struct GraphicsOperation
{
    enum Type
    {
        Add,
        Remove,
        Update
    };

    Type type;
    uint32_t key;
    void *vertexData;
    size_t vertexDataSize;
    std::optional<std::vector<uint16_t>> indices;
    std::optional<glm::vec3> transformPosition;
    std::optional<glm::vec3> transformScale;
    std::optional<GraphicsDriver::ElementType> elementType;
    std::optional<glm::vec3> color;

    std::optional<uint32_t> result;

    void (*callback)();
};