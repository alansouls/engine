#pragma once
#include "BaseSceneSerializer.h"

namespace SSGE
{
class SceneSerializerV1 final : public BaseSceneSerializer
{
public:
    ~SceneSerializerV1() override = default;

    auto serialize(std::ostream &stream, const SceneDefinition &definition) -> void override;
    auto deserialize(std::istream &stream) -> SceneDefinition override;
    [[nodiscard]] auto version() const -> std::array<uint8_t, 3> override;
};
} // SSGE