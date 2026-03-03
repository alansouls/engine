#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <vector>

namespace SSGE
{

struct ComponentFieldDefinition
{
    std::string name;
    std::string value;
};

struct ComponentDefinition
{
    enum ComponentType
    {
        QuadRenderer,
        CircleRenderer,
        QuadCollider,
        CircleCollider,
        Script
    };

    std::string name;
    ComponentType type;
    std::vector<ComponentFieldDefinition> fields;
};

struct TransformDefinition
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

struct GameObjectDefinition
{
    std::string name;
    TransformDefinition transform;
    std::vector<ComponentDefinition> components;
};

struct SceneDefinition
{
    std::string name;
    std::vector<GameObjectDefinition> gameObjects;
};
} // namespace SSGE