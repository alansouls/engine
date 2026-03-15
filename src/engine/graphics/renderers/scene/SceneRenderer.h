#pragma once
#include "SceneCamera.h"
#include "core/Messenger.h"
#include "engine/graphics/drivers/VulkanDriver.h"

class SceneImage;
class RendererItem;
enum RendererItemType : uint32_t;
namespace SSGE
{
class SceneRenderer
{
  public:
    explicit SceneRenderer(VulkanDriver *driver, Messenger *messenger);
    ~SceneRenderer();

    auto addItem(RendererItem *item) -> void;

    virtual auto reset() -> void;

  protected:
    auto render(uint32_t frameIndex, const Resolution &resolution, VkFence fence, VkFramebuffer frameBuffer,
                VkRenderPass renderPass, const std::vector<VkSemaphore> &waitSemaphores,
                const std::vector<VkSemaphore> &signalSemaphores) -> void;

  private:
    VulkanDriver *m_driver;
    Messenger *m_messenger;
    SceneCamera m_camera;
    VkDescriptorSetLayout m_descriptorSetLayout{};
    std::map<uint32_t, RendererItem *> m_items;
    std::set<RendererItem *> m_addedSet;
    std::set<uint32_t> m_removedSet;
    std::set<uint32_t> m_updatedSet;
    std::map<RendererItemType, std::vector<GraphicElement *>> m_elementsByType;
    std::unordered_map<GraphicsDriver::ElementType, PrimitiveData> m_primitives;
    std::unordered_map<GraphicsDriver::ElementType, GraphicsPipelineInfo> m_pipelineInfos;

    auto init() -> void;

    auto initGraphicsResources() -> void;
    auto cleanupGraphicsResources() -> void;

    auto getAddOrRemoveOperations() -> std::map<RendererItem *, GraphicsOperation>;
    auto getUpdateOperations() -> std::vector<GraphicsOperation>;
    auto handleSceneOperations() -> void;

    static auto updateStorageBuffer(const GraphicElement *element, uint32_t currentImage) -> void;
    static auto itemUpdated(void *thisPtr, uint32_t itemKey) -> void;
    auto itemRemoved(uint32_t key) -> void;

    auto performOperation(GraphicsOperation *operation) -> void;
};
} // namespace SSGE
