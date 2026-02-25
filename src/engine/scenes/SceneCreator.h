#pragma once
#include "Component.h"
#include "Scene.h"
#include "SceneDefinitions.h"

class Game;
namespace SSGE
{
class SceneCreator
{
  public:
    SceneCreator() = delete;

    static auto CreateScene(Game *game, const SceneDefinition &definition) -> Scene *;

    static auto CreateGameObject(Scene *scene, GameObject *parent, const GameObjectDefinition &definition)
        -> GameObject *;
    static auto CreateComponent(GameObject *gameObject, const ComponentDefinition &definition) -> Component *;

  private:
    static auto CreateQuadRendererComponent(GameObject *gameObject) -> Component *;
    static auto CreateCircleRendererComponent(GameObject *gameObject) -> Component *;
    static auto CreateQuadColliderComponent(GameObject *gameObject, const ComponentDefinition &definition)
        -> Component *;
    static auto CreateCircleColliderComponent(GameObject *gameObject, const ComponentDefinition &definition)
        -> Component *;
    static auto CreateScriptComponent(GameObject *gameObject, const ComponentDefinition &definition) -> Component *;
    static auto ApplyComponentField(Component *component, const ComponentFieldDefinition &definition) -> void;



    template <ComponentFieldDataType TDataType>
    static auto ApplyComponentField(ComponentField *field, const std::string &value) -> void;
};
} // namespace SSGE
