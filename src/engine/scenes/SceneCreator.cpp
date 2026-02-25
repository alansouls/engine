//
// Created by Alan Maia on 24/02/2026.
//

#include "SceneCreator.h"

#include "Game.h"
#include "GameObject.h"
#include "collisions/QuadCollider.h"
#include "components/CircleRendererComponent.h"
#include "components/QuadRendererComponent.h"

namespace SSGE
{
auto SceneCreator::CreateScene(Game *game, const SceneDefinition &definition) -> Scene *
{
    Scene *scene = game->addScene(definition.name);

    for (const GameObjectDefinition &gameObjectDef : definition.gameObjects)
    {
        CreateGameObject(scene, gameObjectDef);
    }

    return scene;
}

auto SceneCreator::CreateGameObject(Scene *scene, GameObject *parent, const GameObjectDefinition &definition)
    -> GameObject *
{
    GameObject *gameObject = scene->addGameObject(std::make_unique<GameObject>(definition.name, parent));

    auto &transform = gameObject->getInitialTransform();
    transform.setPosition(definition.transform.position);
    transform.setRotation(definition.transform.rotation);
    transform.setScale(definition.transform.scale);

    for (const ComponentDefinition &componentDef : definition.components)
    {
        CreateComponent(gameObject, componentDef);
    }

    return gameObject;
}

auto SceneCreator::CreateComponent(GameObject *gameObject, const ComponentDefinition &definition) -> Component *
{
    Component *component = nullptr;

    switch (definition.type)
    {
    case ComponentDefinition::QuadRenderer:
        component = CreateQuadRendererComponent(gameObject, definition);
        break;
    case ComponentDefinition::CircleRenderer:
        component = CreateCircleRendererComponent(gameObject, definition);
        break;
    case ComponentDefinition::QuadCollider:
        component = CreateQuadColliderComponent(gameObject, definition);
        break;
    case ComponentDefinition::CircleCollider:
        component = CreateCircleColliderComponent(gameObject, definition);
        break;
    case ComponentDefinition::Script:
        component = CreateScriptComponent(gameObject, definition);
        break;
    default:
        throw std::runtime_error("Unknown component type"); // TODO: gracefully handle errors
    }

    return component;
}

auto SceneCreator::CreateQuadRendererComponent(GameObject *gameObject) -> Component *
{
    return &gameObject->addComponent<QuadRendererComponent>(gameObject);
}

auto SceneCreator::CreateCircleRendererComponent(GameObject *gameObject) -> Component *
{
    return &gameObject->addComponent<CircleRendererComponent>(gameObject);
}

auto SceneCreator::CreateQuadColliderComponent(GameObject *gameObject, const ComponentDefinition &definition)
    -> Component *
{
    auto it = std::find_if(
        definition.fields.begin(), definition.fields.end(),
        [](const ComponentFieldDefinition &fieldDefinition) { return fieldDefinition.name == "IsPrimary"; });

    if (it == definition.fields.end())
    {
        throw std::runtime_error("Collider components require IsPrimary field defintion");
    }

    return &gameObject->addComponent<QuadCollider>(it->value == "true", gameObject);
}

auto SceneCreator::CreateCircleColliderComponent(GameObject *gameObject, const ComponentDefinition &definition) -> Component *
{
    auto it = std::find_if(
        definition.fields.begin(), definition.fields.end(),
        [](const ComponentFieldDefinition &fieldDefinition) { return fieldDefinition.name == "IsPrimary"; });

    if (it == definition.fields.end())
    {
        throw std::runtime_error("Collider components require IsPrimary field defintion");
    }

    return &gameObject->addComponent<QuadRendererComponent>(gameObject);
}

auto SceneCreator::CreateScriptComponent(GameObject *gameObject) -> Component *
{
    return &gameObject->addComponent<QuadRendererComponent>(gameObject);
}

} // namespace SSGE