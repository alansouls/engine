#include <glm/glm.hpp>
#include <optional>

class RendererItem;
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
    std::optional<RendererItem *> item;

    std::optional<uint32_t> result;

    void (*callback)();
};
