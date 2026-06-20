#pragma once
#include "BaseSceneSerializer.h"

#include <array>
#include <glm/vec3.hpp>
#include <iostream>
#include <vector>

namespace SSGE
{
class SceneSerializerV1 : public BaseSceneSerializer
{
  public:
    ~SceneSerializerV1() override = default;

    auto serialize(std::ostream &stream, const SceneDefinition &definition) -> void override;
    auto deserialize(std::istream &stream) -> SceneDefinition override;
    [[nodiscard]] auto version() const -> std::array<uint8_t, 3> override;

  protected:
    virtual auto deserializeGameObjects(std::istream &stream) -> std::vector<GameObjectDefinition>;
    virtual auto deserializeGameObject(std::istream &stream) -> GameObjectDefinition;
    virtual auto deserializeComponents(std::istream &stream) -> std::vector<ComponentDefinition>;
    virtual auto deserializeComponent(std::istream &stream) -> ComponentDefinition;
    virtual auto deserializeComponentFields(std::istream &stream) -> std::vector<ComponentFieldDefinition>;
    virtual auto deserializeComponentField(std::istream &stream) -> ComponentFieldDefinition;
    virtual auto deserializeTransform(std::istream &stream) -> TransformDefinition;
    virtual auto deserializeUInt32T(std::istream &stream, const std::string_view &fieldName) -> uint32_t;
    virtual auto deserializeVector3(std::istream &stream, const std::string_view &fieldName) -> glm::vec3;
    virtual auto expectHeader(std::istream &stream, const std::string_view &header) const -> void;
    virtual auto deserializeString(std::istream &stream, const std::string_view &fieldName) -> std::string;
};
} // namespace SSGE
