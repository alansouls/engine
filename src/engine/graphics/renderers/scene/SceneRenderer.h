#pragma once
#include "SceneCamera.h"
#include "engine/graphics/drivers/VulkanDriver.h"

class SceneImage;
class RendererItem;
enum RendererItemType : uint32_t;
namespace SSGE
{
class SceneRenderer
{
  public:
    explicit SceneRenderer(VulkanDriver *driver);
    ~SceneRenderer();

    auto render(SceneCamera *camera, const Resolution &resolution, VkFence fence, VkFramebuffer frameBuffer,
                VkCommandBuffer commandBuffer, VkRenderPass renderPass) -> void;

  private:
    VulkanDriver *m_driver;
    std::map<uint32_t, RendererItem *> m_items;
    std::set<RendererItem *> m_addedSet;
    std::set<uint32_t> m_removedSet;
    std::set<uint32_t> m_updatedSet;
    std::map<RendererItemType, std::vector<GraphicElement *>> m_elementsByType;

    auto getAddOrRemoveOperations() -> std::map<RendererItem *, GraphicsOperation>;
    auto getUpdateOperations() -> std::vector<GraphicsOperation>;
    auto handleSceneOperations(SceneCamera *camera) -> void;

    static auto updateStorageBuffer(const GraphicElement *element, uint32_t currentImage) -> void;
    static auto itemUpdated(void *thisPtr, uint32_t itemKey) -> void;
    auto performOperation(GraphicsOperation *operation, SceneCamera *camera) -> void;
};
} // namespace SSGE