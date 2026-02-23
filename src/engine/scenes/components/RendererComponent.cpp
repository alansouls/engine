#include "RendererComponent.h"

#include "engine/scenes/Game.h"
#include "engine/scenes/GameObject.h"

namespace SSGE
{
RendererComponent::RendererComponent(GameObject *gameObject, std::unique_ptr<RendererItem> item, std::string name,
                                     std::string displayName)
    : Component(std::move(name), std::move(displayName), gameObject), m_changed(false), m_item(std::move(item)),
      m_gameObject(gameObject)
{
    m_item->bindWorldTransform(gameObject->getConstTransform().getMatrix());

    auto game = Game::getInstance();

    if (!game)
    {
        throw std::runtime_error("Game instance is not initialized.");
    }

    game->getRenderer().addItem(m_item.get());
}

auto RendererComponent::init() -> void
{
}

auto RendererComponent::update() -> void
{
}

} // namespace SSGE