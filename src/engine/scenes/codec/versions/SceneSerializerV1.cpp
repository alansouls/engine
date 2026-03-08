#include "SceneSerializerV1.h"

#include <array>
#include <cstring>
#include <format>

namespace SSGE
{

auto SceneSerializerV1::serialize(std::ostream &stream, const SceneDefinition &definition) -> void
{
    stream.write(definition.name.data(), static_cast<std::streamsize>(definition.name.size() + 1));
    uint32_t gameObjectsCount = definition.gameObjects.size();
    stream.write(reinterpret_cast<char *>(&gameObjectsCount), sizeof(uint32_t));
    for (auto gameObject : definition.gameObjects)
    {
        stream.write(gameObject.name.data(), static_cast<std::streamsize>(gameObject.name.size() + 1));
        stream.write(reinterpret_cast<char *>(&gameObject.transform), sizeof(TransformDefinition));
        uint32_t componentCount = gameObject.components.size();
        stream.write(reinterpret_cast<char *>(&componentCount), sizeof(uint32_t));
        for (auto component : gameObject.components)
        {
            stream.write(component.name.data(), static_cast<std::streamsize>(component.name.size() + 1));
            stream.write(reinterpret_cast<char *>(&component.type), sizeof(ComponentDefinition::ComponentType));
            uint32_t fieldsCount = component.fields.size();
            stream.write(reinterpret_cast<char *>(&fieldsCount), sizeof(uint32_t));
            for (auto field : component.fields)
            {
                stream.write(field.name.data(), static_cast<std::streamsize>(field.name.size() + 1));
                stream.write(field.value.data(), static_cast<std::streamsize>(field.value.size() + 1));
            }
        }
    }
}

auto SceneSerializerV1::deserialize(std::istream &stream) -> SceneDefinition
{
    StreamReader reader(stream);
    return SceneDefinition{
        .name = deserializeString(reader, "Scene Name"),
        .gameObjects = deserializeGameObjects(reader),
    };
}

auto SceneSerializerV1::deserializeGameObjects(StreamReader &stream) -> std::vector<GameObjectDefinition>
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

auto SceneSerializerV1::deserializeGameObject(StreamReader &stream) -> GameObjectDefinition
{
    return GameObjectDefinition{
        .name = deserializeString(stream, "GameObject Name"),
        .transform = deserializeTransform(stream),
        .components = deserializeComponents(stream),
    };
}

auto SceneSerializerV1::deserializeComponents(StreamReader &stream) -> std::vector<ComponentDefinition>
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

auto SceneSerializerV1::deserializeComponent(StreamReader &stream) -> ComponentDefinition
{
    return ComponentDefinition{
        .name = deserializeString(stream, "Component Name"),
        .type = static_cast<ComponentDefinition::ComponentType>(deserializeUInt32T(stream, "Component Type")),
        .fields = deserializeComponentFields(stream),
    };
}

auto SceneSerializerV1::deserializeComponentFields(StreamReader &stream) -> std::vector<ComponentFieldDefinition>
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

auto SceneSerializerV1::deserializeComponentField(StreamReader &stream) -> ComponentFieldDefinition
{
    return ComponentFieldDefinition{
        .name = deserializeString(stream, "Component Field Name"),
        .value = deserializeString(stream, "Component Field Value"),
    };
}

auto SceneSerializerV1::deserializeTransform(StreamReader &stream) -> TransformDefinition
{
    return TransformDefinition{
        .position = deserializeVector3(stream, "Transform Position"),
        .rotation = deserializeVector3(stream, "Transform Rotation"),
        .scale = deserializeVector3(stream, "Transform Scale"),
    };
}

auto SceneSerializerV1::deserializeVector3(StreamReader &stream, const std::string_view &fieldName) -> glm::vec3
{
    int32_t bytesRead = sizeof(glm::vec3);
    auto resultBuffer = stream.readNext(bytesRead);
    if (bytesRead != sizeof(glm::vec3))
    {
        throw std::runtime_error(std::format("Could not read {} from stream", fieldName));
    }

    glm::vec3 result;
    std::memcpy(reinterpret_cast<void *>(&result), resultBuffer.data(), sizeof(result));
    return result;
}

auto SceneSerializerV1::deserializeUInt32T(StreamReader &stream, const std::string_view &fieldName) -> uint32_t
{
    int32_t bytesRead = sizeof(uint32_t);
    auto resultBuffer = stream.readNext(bytesRead);
    if (bytesRead != sizeof(uint32_t))
    {
        throw std::runtime_error(std::format("Could not read {} from stream", fieldName));
    }

    uint32_t result;
    std::memcpy(reinterpret_cast<void *>(&result), resultBuffer.data(), sizeof(result));
    return result;
}

auto SceneSerializerV1::deserializeString(StreamReader &stream, const std::string_view &fieldName) -> std::string
{
    constexpr int32_t stringChunk = 255;
    std::string resultString;
    std::vector<char> stringBytes;
    stringBytes.reserve(stringChunk);
    while (resultString.empty())
    {
        int32_t bytesRead = stringChunk;
        std::span<std::byte> readBuffer = stream.readNext(bytesRead);
        if (bytesRead == 0)
        {
            throw std::runtime_error(std::format("Could not read {} from stream", fieldName));
        }
        auto begin = readBuffer.begin();
        auto nullTerminator = std::ranges::find(readBuffer, static_cast<std::byte>(0));
        int offset = 0;
        while (begin != nullTerminator)
        {
            offset++;
            stringBytes.push_back(static_cast<char>(*begin++));
        }

        if (nullTerminator != readBuffer.end())
        {
            stream.returnPos(static_cast<int64_t>(bytesRead) - (offset + 1));
            resultString = std::string(stringBytes.begin(), stringBytes.end());
            break;
        }
    }
    return resultString;
}

auto SceneSerializerV1::version() const -> std::array<uint8_t, 3>
{
    return {1, 0, 0};
}

} // namespace SSGE
