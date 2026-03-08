#pragma once

#include "scenes/Component.h"
#include "scenes/ComponentField.h"
#include "scenes/GameObject.h"
#include "scenes/Scene.h"
#include <glm/vec3.hpp>
#include <ranges>
#include <string>
#include <vector>

namespace SSGE
{

struct ComponentFieldDefinition
{
    std::string name;
    std::string value;

    static auto FromInstance(const ComponentField *field) -> ComponentFieldDefinition
    {
        return ComponentFieldDefinition{.name = field->name(), .value = field->valueAsString()};
    }
};

struct ComponentDefinition
{

    std::string name;
    Component::ComponentType type;
    std::vector<ComponentFieldDefinition> fields;

    static auto FromInstance(const Component *component) -> ComponentDefinition
    {
        std::vector<ComponentFieldDefinition> fields;
        std::vector<ComponentField *> componentFields = component->getComponentFields();
        fields.reserve(componentFields.size());
        for (auto field : componentFields)
        {
            fields.push_back(ComponentFieldDefinition::FromInstance(field));
        }

        return ComponentDefinition{
            .name = component->name(),
            .type = component->type(),
            .fields = std::move(fields),
        };
    }
};

struct TransformDefinition
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    static auto FromInstance(const Transform &transform) -> TransformDefinition
    {
        return TransformDefinition{
            .position = transform.getPosition(),
            .rotation = transform.getRotation(),
            .scale = transform.getScale(),
        };
    }
};

struct GameObjectDefinition
{
    std::string name;
    TransformDefinition transform;
    std::vector<ComponentDefinition> components;

    static auto FromInstance(const GameObject *gameObject) -> GameObjectDefinition
    {
        std::vector<ComponentDefinition> components;
        std::vector<Component *> gameObjectComponents = gameObject->components();
        components.reserve(gameObjectComponents.size());
        for (auto component : gameObjectComponents)
        {
            components.push_back(ComponentDefinition::FromInstance(component));
        }
        return GameObjectDefinition{
            .name = gameObject->name(),
            .transform = TransformDefinition::FromInstance(gameObject->getConstTransform()),
            .components = std::move(components),
        };
    }
};

struct SceneDefinition
{
    std::string name;
    std::vector<GameObjectDefinition> gameObjects;

    static auto FromInstance(const Scene *scene) -> SceneDefinition
    {
        auto definitionViews = scene->gameObjects() | std::views::transform([](GameObject *gameObject) {
                                   return GameObjectDefinition::FromInstance(gameObject);
                               });

        return SceneDefinition{
            .name = scene->getName(),
            .gameObjects = std::vector(definitionViews.begin(), definitionViews.end()),
        };
    }
};
} // namespace SSGE
