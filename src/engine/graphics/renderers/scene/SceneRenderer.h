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

    auto render(uint32_t frameIndex, const Resolution &resolution, VkFence fence, VkFramebuffer frameBuffer,
                VkRenderPass renderPass, const std::vector<VkSemaphore> &waitSemaphores,
                const std::vector<VkSemaphore> &signalSemaphores) -> void;

  private:
    VulkanDriver *m_driver;
    SceneCamera m_camera;
    VkDescriptorSetLayout m_descriptorSetLayout{};
    std::map<uint32_t, RendererItem *> m_items;
    std::set<RendererItem *> m_addedSet;
    std::set<uint32_t> m_removedSet;
    std::set<uint32_t> m_updatedSet;
    std::map<RendererItemType, std::vector<GraphicElement *>> m_elementsByType;

    auto initGraphicsResources() -> void;
    auto cleanupGraphicsResources() -> void;

    auto getAddOrRemoveOperations() -> std::map<RendererItem *, GraphicsOperation>;
    auto getUpdateOperations() -> std::vector<GraphicsOperation>;
    auto handleSceneOperations() -> void;

    static auto updateStorageBuffer(const GraphicElement *element, uint32_t currentImage) -> void;
    auto addItem(RendererItem *item) -> void;
    static auto itemUpdated(void *thisPtr, uint32_t itemKey) -> void;
    auto performOperation(GraphicsOperation *operation) -> void;
};
} // namespace SSGE