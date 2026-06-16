#include "SceneSerializerV1.h"

#include <array>

namespace SSGE
{

inline auto valueAsString(const glm::vec3 &vec) -> std::string
{
    return std::to_string(vec.x) + "|" + std::to_string(vec.y) + "|" + std::to_string(vec.z);
}

auto SceneSerializerV1::serialize(std::ostream &stream, const SceneDefinition &definition) -> void
{
    stream << "[Scene]\n";
    stream << definition.name << '\n';
    stream << "[GameObjects]\n";
    for (auto gameObject : definition.gameObjects)
    {
        stream << "[GameObject]\n";
        stream << gameObject.name << '\n';
        stream << "[Transform]\n";
        stream << "[Position]\n";
        stream << valueAsString(gameObject.transform.position) << '\n';
        stream << "[Rotation]\n";
        stream << valueAsString(gameObject.transform.rotation) << '\n';
        stream << "[Scale]\n";
        stream << valueAsString(gameObject.transform.scale) << '\n';
        stream << "[Components]\n";
        for (auto component : gameObject.components)
        {
            stream << "[Component]\n";
            stream << component.name << '\n';
            stream << (int)component.type << '\n';
            stream << "[Fields]\n";
            for (auto field : component.fields)
            {
                stream << "[Field]\n";
                stream << field.name << '\n';
                stream << field.value << '\n';
            }
        }
    }
}

auto SceneSerializerV1::deserialize(std::istream &stream) -> SceneDefinition
{
    return SceneDefinition{
        .name = "",
        .gameObjects = deserializeGameObjects(stream),
    };
}

auto SceneSerializerV1::deserializeGameObjects(std::istream &stream) -> std::vector<GameObjectDefinition>
{
    uint32_t gameObjectsCount = deserializeUInt32T(stream, "Number of GameObjects");
    std::vector<GameObjectDefinition> gameObjects;
    gameObjects.resize(gameObjectsCount);
    for (uint32_t i = 0; i < gameObjectsCount; i++)
    {
        gameObjects[i] = deserializeGameObject(stream);
    }

    return gameObjects;
}

auto SceneSerializerV1::deserializeGameObject(std::istream &stream) -> GameObjectDefinition
{
    return GameObjectDefinition{
        .name = "",
        .transform = deserializeTransform(stream),
        .components = deserializeComponents(stream),
    };
}

auto SceneSerializerV1::deserializeComponents(std::istream &stream) -> std::vector<ComponentDefinition>
{
    uint32_t componentsCount = deserializeUInt32T(stream, "Number of Components");
    std::vector<ComponentDefinition> components;
    components.resize(componentsCount);
    for (uint32_t i = 0; i < componentsCount; i++)
    {
        components[i] = deserializeComponent(stream);
    }

    return components;
}

auto SceneSerializerV1::deserializeComponent(std::istream &stream) -> ComponentDefinition
{
    return ComponentDefinition{
        .name = "",
        .type = static_cast<Component::ComponentType>(deserializeUInt32T(stream, "Component Type")),
        .fields = deserializeComponentFields(stream),
    };
}

auto SceneSerializerV1::deserializeComponentFields(std::istream &stream) -> std::vector<ComponentFieldDefinition>
{
    uint32_t fieldsCount = deserializeUInt32T(stream, "Number of Component Fields");
    std::vector<ComponentFieldDefinition> fields;
    fields.resize(fieldsCount);
    for (uint32_t i = 0; i < fieldsCount; i++)
    {
        fields[i] = deserializeComponentField(stream);
    }

    return fields;
}

auto SceneSerializerV1::deserializeComponentField(std::istream &stream) -> ComponentFieldDefinition
{
    return ComponentFieldDefinition{
        .name = "",
        .value = "",
    };
}

auto SceneSerializerV1::deserializeTransform(std::istream &stream) -> TransformDefinition
{
    return TransformDefinition{
        .position = deserializeVector3(stream, "Transform Position"),
        .rotation = deserializeVector3(stream, "Transform Rotation"),
        .scale = deserializeVector3(stream, "Transform Scale"),
    };
}

auto SceneSerializerV1::deserializeVector3(std::istream &stream, const std::string_view &fieldName) -> glm::vec3
{
    return {};
    // int32_t bytesRead = sizeof(glm::vec3);
    // auto resultBuffer = stream.readNext(bytesRead);
    // if (bytesRead != sizeof(glm::vec3))
    // {
    //     throw std::runtime_error(std::format("Could not read {} from stream", fieldName));
    // }
    //
    // glm::vec3 result;
    // std::memcpy(reinterpret_cast<void *>(&result), resultBuffer.data(), sizeof(result));
    // return result;
}

auto SceneSerializerV1::deserializeUInt32T(std::istream &stream, const std::string_view &fieldName) -> uint32_t
{
    return 0;
    // int32_t bytesRead = sizeof(uint32_t);
    // auto resultBuffer = stream.readNext(bytesRead);
    // if (bytesRead != sizeof(uint32_t))
    // {
    //     throw std::runtime_error(std::format("Could not read {} from stream", fieldName));
    // }
    //
    // uint32_t result;
    // std::memcpy(reinterpret_cast<void *>(&result), resultBuffer.data(), sizeof(result));
    // return result;
}

auto SceneSerializerV1::version() const -> std::array<uint8_t, 3>
{
    return {1, 0, 0};
}

} // namespace SSGE
