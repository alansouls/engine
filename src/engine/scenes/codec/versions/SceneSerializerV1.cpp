#include "SceneSerializerV1.h"
#include "utils/StringUtils.h"

#include <array>
#include <format>
#include <glm/ext/vector_float3.hpp>
#include <istream>
#include <ranges>
#include <stdexcept>
#include <string>

namespace SSGE
{

inline auto valueAsString(const glm::vec3 &vec) -> std::string
{
    return std::format("{:.6f}|{:.6f}|{:.6f}", vec.x, vec.y, vec.z);
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
            stream << "[Fields]\n";
        }
        stream << "[Components]\n";
    }
    stream << "[GameObjects]\n";
}

auto SceneSerializerV1::deserialize(std::istream &stream) -> SceneDefinition
{
    expectHeader(stream, "[Scene]");

    std::string sceneName = deserializeString(stream, "Scene Name");

    return SceneDefinition{
        .name = sceneName,
        .gameObjects = deserializeGameObjects(stream),
    };
}

auto SceneSerializerV1::deserializeGameObjects(std::istream &stream) -> std::vector<GameObjectDefinition>
{
    expectHeader(stream, "[GameObjects]");
    std::vector<GameObjectDefinition> gameObjects;
    while (true)
    {
        std::string nextHeader;
        if (!std::getline(stream, nextHeader) || (nextHeader != "[GameObjects]" && nextHeader != "[GameObject]"))
        {
            throw std::runtime_error("Could read not next header for game objects");
        }

        if (nextHeader == "[GameObjects]")
        {
            break;
        }

        gameObjects.push_back(deserializeGameObject(stream));
    }

    return gameObjects;
}

auto SceneSerializerV1::deserializeGameObject(std::istream &stream) -> GameObjectDefinition
{
    return GameObjectDefinition{
        .name = deserializeString(stream, "GameObject Name"),
        .transform = deserializeTransform(stream),
        .components = deserializeComponents(stream),
    };
}

auto SceneSerializerV1::deserializeComponents(std::istream &stream) -> std::vector<ComponentDefinition>
{
    expectHeader(stream, "[Components]");
    std::vector<ComponentDefinition> components;
    while (true)
    {
        std::string nextHeader;
        if (!std::getline(stream, nextHeader) || (nextHeader != "[Components]" && nextHeader != "[Component]"))
        {
            throw std::runtime_error("Could read not next header for components");
        }

        if (nextHeader == "[Components]")
        {
            break;
        }

        components.push_back(deserializeComponent(stream));
    }

    return components;
}

auto SceneSerializerV1::deserializeComponent(std::istream &stream) -> ComponentDefinition
{
    return ComponentDefinition{
        .name = deserializeString(stream, "Component Name"),
        .type = static_cast<Component::ComponentType>(deserializeUInt32T(stream, "Component Type")),
        .fields = deserializeComponentFields(stream),
    };
}

auto SceneSerializerV1::deserializeComponentFields(std::istream &stream) -> std::vector<ComponentFieldDefinition>
{
    expectHeader(stream, "[Fields]");
    std::vector<ComponentFieldDefinition> fields;
    while (true)
    {
        std::string nextHeader = deserializeString(stream, "Field Header");
        if (nextHeader != "[Fields]" && nextHeader != "[Field]")
        {
            throw std::runtime_error("Unexpected value for field header");
        }

        if (nextHeader == "[Fields]")
        {
            break;
        }

        fields.push_back(deserializeComponentField(stream));
    }

    return fields;
}

auto SceneSerializerV1::deserializeComponentField(std::istream &stream) -> ComponentFieldDefinition
{
    return ComponentFieldDefinition{
        .name = deserializeString(stream, "Field Name"),
        .value = deserializeString(stream, "Field Value"),
    };
}

auto SceneSerializerV1::deserializeTransform(std::istream &stream) -> TransformDefinition
{
    expectHeader(stream, "[Transform]");

    expectHeader(stream, "[Position]");
    glm::vec3 position = deserializeVector3(stream, "Transform Position");

    expectHeader(stream, "[Rotation]");
    glm::vec3 rotation = deserializeVector3(stream, "Transform Rotation");

    expectHeader(stream, "[Scale]");
    glm::vec3 scale = deserializeVector3(stream, "Transform Scale");

    return TransformDefinition{
        .position = position,
        .rotation = rotation,
        .scale = scale,
    };
}

auto SceneSerializerV1::deserializeVector3(std::istream &stream, const std::string_view &fieldName) -> glm::vec3
{
    std::string vec3Line = deserializeString(stream, fieldName);

    float result[3] = {};
    int i = 0;
    for (auto part : StringUtils::Split(vec3Line, "|"))
    {
        result[i++] = std::stof(std::string(part));
    }

    return {result[0], result[1], result[2]};
}

auto SceneSerializerV1::deserializeUInt32T(std::istream &stream, const std::string_view &fieldName) -> uint32_t
{
    std::string strValue;
    if (!std::getline(stream, strValue))
    {
        throw std::runtime_error(std::format("Could not read line for {}", fieldName));
    }

    return std::stoul(strValue);
}

auto SceneSerializerV1::version() const -> std::array<uint8_t, 3>
{
    return {1, 0, 0};
}

auto SceneSerializerV1::expectHeader(std::istream &stream, const std::string_view &header) const -> void
{
    std::string readHeader;
    if (!std::getline(stream, readHeader) || readHeader != header)
    {
        throw std::runtime_error(std::format("Could not found expected header: {}", header));
    }
}

auto SceneSerializerV1::deserializeString(std::istream &stream, const std::string_view &fieldName) -> std::string
{
    std::string value;
    if (!std::getline(stream, value))
    {
        throw std::runtime_error(std::format("Could not read string for {}", fieldName));
    }

    return value;
}

} // namespace SSGE
