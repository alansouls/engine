#pragma once

#include "../../drivers/VulkanDriver.h"

class RendererItem;
class SceneRenderer
{
  public:
    SceneRenderer(VulkanDriver *driver);

    auto render() -> SceneImage;

  private:
    VulkanDriver *m_driver;
    std::map<uint32_t, RendererItem *> m_items;
    std::set<RendererItem *> m_addedSet;
    std::set<uint32_t> m_removedSet;
    std::set<uint32_t> m_updatedSet;

    std::map<RendererItem *, GraphicsOperation> getAddOrRemoveOperations();
    std::vector<GraphicsOperation> getUpdateOperations();

    friend class RendererItem;

    static void itemUpdated(void *thisPtr, uint32_t itemKey);
};
