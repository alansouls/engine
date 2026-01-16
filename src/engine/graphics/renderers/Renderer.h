#pragma once

#include "../EngineWindow.h"
#include "../drivers/GraphicsOperation.h"
#include "../utils/Vertex.h"
#include "ui/UIRenderer.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <map>
#include <set>

struct RendererOptions
{
    bool debugModeOn = false;
    std::optional<uint32_t> fpsCap;
};

class EditorSceneRenderer;
class GraphicsDriver;
class RectangleItem;
class RendererItem;
class Renderer
{
  public:
    Renderer(EngineWindow *mainWindow, const RendererOptions &options);
    ~Renderer();

    void render();

    [[nodiscard]] auto getWidth() const -> uint32_t;

    [[nodiscard]] auto getHeight() const -> uint32_t;

    [[nodiscard]] auto getSceneWidth() const -> uint32_t;
    [[nodiscard]] auto getSceneHeight() const -> uint32_t;

    auto addItem(RendererItem *item) const -> void;

  private:
    EngineWindow *m_window;
    RendererOptions m_options;

    VulkanDriver *m_driver;
    std::unique_ptr<UIRenderer> m_uiRenderer;
    std::unique_ptr<EditorSceneRenderer> m_sceneRenderer;

    void initGraphicsDriver();

    static std::vector<const char *> getVulkanRequiredExtensions();

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    void setDimensions();

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_currentImage;
};
