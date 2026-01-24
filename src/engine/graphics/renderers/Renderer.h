#pragma once
#include "graphics/EngineWindow.h"
#include "scene/RendererItem.h"

#include <optional>

namespace SSGE
{

struct RendererOptions
{
    bool debugModeOn = false;
    std::optional<uint32_t> fpsCap;
};

class Renderer
{
  public:
    Renderer(EngineWindow *window, const RendererOptions &options);
    virtual ~Renderer() = default;

    auto render() -> void;
    auto addItem(RendererItem *item) const -> void;

    [[nodiscard]] auto getWidth() const -> uint32_t;

    [[nodiscard]] auto getHeight() const -> uint32_t;

  protected:
    std::unique_ptr<VulkanDriver> m_driver;
    EngineWindow *m_window;
    RendererOptions m_options;
    std::vector<SceneRenderer *> m_sceneRenderers;

    virtual auto preRender(uint32_t currentFrame) -> void;
    virtual auto drawFrame(uint32_t frameIndex) -> void = 0;
    virtual auto postRender(uint32_t currentFrame) -> void;

  private:
    uint32_t m_currentImage;

    auto initWindow(EngineWindow *mainWindow) -> void;
    auto initGraphicsDriver() -> void;

    static std::vector<const char *> getVulkanRequiredExtensions();

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
};

} // namespace SSGE
