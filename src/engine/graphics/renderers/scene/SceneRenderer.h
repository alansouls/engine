#pragma once

#include "../../drivers/VulkanDriver.h"
#include "SceneImage.h"

typedef MappedBuffer Camera;

class RendererItem;
enum RendererItemType : uint32_t;
class SceneRenderer
{
public:
    explicit SceneRenderer(VulkanDriver *driver, uint32_t width, uint32_t height);
    ~SceneRenderer();

    auto render(uint32_t currentImage) -> std::shared_ptr<SceneImage>;

    auto resize(uint32_t width, uint32_t height) -> void;

    auto addItem(RendererItem* item) -> void;

    [[nodiscard]] auto getWidth() const -> uint32_t;
    [[nodiscard]]  auto getHeight() const -> uint32_t;

  private:
    VulkanDriver *m_driver;
    std::map<uint32_t, RendererItem *> m_items;
    std::set<RendererItem *> m_addedSet;
    std::set<uint32_t> m_removedSet;
    std::set<uint32_t> m_updatedSet;
    std::map<RendererItemType, std::vector<GraphicElement *>> m_elementsByType;
    VkRenderPass m_renderPass;
    VkDescriptorSetLayout m_descriptorSetLayout{};
    std::array<std::shared_ptr<SceneImage>, MAX_FRAMES_IN_FLIGHT> m_images;
    std::array<VkFramebuffer, MAX_FRAMES_IN_FLIGHT> m_framebuffers;
    std::array<Camera, MAX_FRAMES_IN_FLIGHT> m_cameras;

    auto init(uint32_t width, uint32_t height) -> void;
    auto getAddOrRemoveOperations() -> std::map<RendererItem *, GraphicsOperation>;
    auto getUpdateOperations() -> std::vector<GraphicsOperation>;
    auto handleSceneOperations() -> void;

    friend class RendererItem;
    auto performOperation(GraphicsOperation *operation) -> void;

    auto updateCameraBuffer(uint32_t currentImage) const -> void;
    static auto updateStorageBuffer(const GraphicElement *element, uint32_t currentImage) -> void;
    static auto itemUpdated(void *thisPtr, uint32_t itemKey) -> void;

    uint32_t m_resizeWidth[MAX_FRAMES_IN_FLIGHT];
    uint32_t m_resizeHeight[MAX_FRAMES_IN_FLIGHT];
    auto commitResize(uint32_t currentImage) -> void;
};
