#include "RendererComponent.h"

#include "engine/scenes/Game.h"

namespace SSGE
{
RendererComponent::RendererComponent(GameObject *gameObject, std::unique_ptr<RendererItem> item, const std::string &typeName)
    : Component(typeName, gameObject), m_changed(false), m_item(std::move(item)), m_gameObject(gameObject)
{
}

auto RendererComponent::init() -> void
{
    auto game = Game::getInstance();

    if (!game)
    {
        throw std::runtime_error("Game instance is not initialized.");
    }

    game->getRenderer().addItem(m_item.get());
}

auto RendererComponent::update() -> void
{
    //TODO move renderer item update logic here
}

} // namespace SSGE