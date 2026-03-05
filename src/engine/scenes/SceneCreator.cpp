//
// Created by Alan Maia on 24/02/2026.
//

#include "SceneCreator.h"

#include "Game.h"
#include "GameObject.h"
#include "collisions/CircleCollider.h"
#include "collisions/QuadCollider.h"
#include "components/CircleRendererComponent.h"
#include "components/QuadRendererComponent.h"
#include "scripts/components/ScriptComponent.h"
#include "utils/ParseUtils.h"
#include "utils/StringUtils.h"

namespace SSGE
{

auto SceneCreator::CreateScene(Game *game, const SceneDefinition &definition) -> Scene *
{
    Scene *scene = game->addScene(definition.name);

    for (const GameObjectDefinition &gameObjectDef : definition.gameObjects)
    {
        CreateGameObject(scene, nullptr, gameObjectDef);
    }

    game->setCurrentScene(scene->getName());

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
    case ComponentDefinition::ComponentType::QuadRenderer:
        component = CreateQuadRendererComponent(gameObject);
        break;
    case ComponentDefinition::ComponentType::CircleRenderer:
        component = CreateCircleRendererComponent(gameObject);
        break;
    case ComponentDefinition::ComponentType::QuadCollider:
        component = CreateQuadColliderComponent(gameObject, definition);
        break;
    case ComponentDefinition::ComponentType::CircleCollider:
        component = CreateCircleColliderComponent(gameObject, definition);
        break;
    case ComponentDefinition::ComponentType::Script:
        component = CreateScriptComponent(gameObject, definition);
        break;
    default:
        throw std::runtime_error("Unknown component type"); // TODO: gracefully handle errors
    }

    for (auto &field : definition.fields)
    {
        ApplyComponentField(component, field);
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
    const auto it = std::ranges::find_if(definition.fields, [](const ComponentFieldDefinition &fieldDefinition) {
        return fieldDefinition.name == "IsPrimary";
    });

    if (it == definition.fields.end())
    {
        throw std::runtime_error("Collider components require IsPrimary field definition");
    }

    return &gameObject->addComponent<QuadCollider>(it->value == "T", gameObject);
}

auto SceneCreator::CreateCircleColliderComponent(GameObject *gameObject, const ComponentDefinition &definition)
    -> Component *
{
    auto it = std::ranges::find_if(definition.fields, [](const ComponentFieldDefinition &fieldDefinition) {
        return fieldDefinition.name == "IsPrimary";
    });

    if (it == definition.fields.end())
    {
        throw std::runtime_error("Collider components require IsPrimary field definition");
    }

    return &gameObject->addComponent<CircleCollider>(it->value == "T", gameObject);
}

auto SceneCreator::CreateScriptComponent(GameObject *gameObject, const ComponentDefinition &definition) -> Component *
{
    return &gameObject->addComponent<ScriptComponent>(gameObject, definition.name);
}

auto SceneCreator::ApplyComponentField(Component *component, const ComponentFieldDefinition &definition) -> void
{
    const std::vector<ComponentField *> fields = component->getComponentFields();
    const auto it = std::ranges::find_if(
        fields, [definition](const ComponentField *field) { return definition.name == field->name(); });

    if (it == fields.end())
    {
        return;
    }

    auto field = *it;

    switch (field->type())
    {
    case ComponentField::Int:
        ApplyComponentField<int>(field, definition.value);
        break;
    case ComponentField::Float:
        ApplyComponentField<float>(field, definition.value);
        break;
    case ComponentField::String:
        ApplyComponentField<std::string>(field, definition.value);
        break;
    case ComponentField::Bool:
        ApplyComponentField<bool>(field, definition.value);
        break;
    case ComponentField::Vec2:
        ApplyComponentField<glm::vec2>(field, definition.value);
        break;
    case ComponentField::Vec3:
        ApplyComponentField<glm::vec3>(field, definition.value);
        break;
    case ComponentField::Vec4:
    case ComponentField::Color:
        ApplyComponentField<glm::vec4>(field, definition.value);
        break;
    default:
        throw std::runtime_error("Unknown component type");
    }
}

template <ComponentFieldDataType TDataType>
auto SceneCreator::ApplyComponentField(ComponentField *field, const std::string &value) -> void
{
    auto parsedValue = ParseUtils::Parse<TDataType>(value);

    auto typedField = dynamic_cast<TypedComponentField<TDataType> *>(field);

    if (!typedField)
    {
        throw std::runtime_error("Invalid component type");
    }

    typedField->setCurrentValue(parsedValue);
}
} // namespace SSGE
