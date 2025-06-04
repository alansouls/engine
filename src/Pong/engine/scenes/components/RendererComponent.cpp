#include "RendererComponent.h"

#include "engine/scenes/Game.h"

namespace SSGE
{

RendererComponent::RendererComponent(GameObject *gameObject, RendererItem *item)
    : Component(TypeName), m_changed(false), m_gameObject(gameObject), m_item(item)
{
}

auto RendererComponent::init() -> void
{
    auto game = Game::getInstance();

    if (!game)
    {
        throw std::runtime_error("Game instance is not initialized.");
    }

    game->getRenderer().addItem(m_item);
}

auto RendererComponent::update() -> void
{
    //TODO move renderer item update logic here
}

auto RendererComponent::gameObject() -> GameObject &
{
    return *m_gameObject;
}

} // namespace SSGE