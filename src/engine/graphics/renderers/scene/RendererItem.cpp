#include "RendererItem.h"

#include "scenes/Game.h"

RendererItem::~RendererItem()
{
    SSGE::Messenger *messenger = Game::getInstance()->messenger();
    messenger->send(ItemDeletedMessage{.key = m_key});
}
