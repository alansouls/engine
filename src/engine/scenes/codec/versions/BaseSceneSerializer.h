#pragma once

#include "scenes/SceneDefinitions.h"

#include <ostream>

namespace SSGE
{
class BaseSceneSerializer
{
public:
    virtual ~BaseSceneSerializer() = default;
    virtual auto serialize(std::ostream &stream, const SceneDefinition &definition) -> void = 0;
    virtual auto deserialize(std::istream &stream) -> SceneDefinition = 0;
    [[nodiscard]] virtual auto version() const -> std::array<uint8_t, 3> = 0;
};
}