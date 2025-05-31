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

class SceneRenderer;
class GraphicsDriver;
class RectangleItem;
class RendererItem;
class Renderer
{
  public:
    Renderer(EngineWindow *mainWindow, const RendererOptions &options);
    ~Renderer();

    void render();

    [[nodiscard]] uint32_t getWidth() const
    {
        return m_width;
    }

    [[nodiscard]] uint32_t getHeight() const
    {
        return m_height;
    }

    void addItem(RendererItem *item) const;

  private:
    EngineWindow *m_window;
    RendererOptions m_options;

    VulkanDriver *m_driver;
    std::unique_ptr<UIRenderer> m_uiRenderer;
    std::unique_ptr<SceneRenderer> m_sceneRenderer;

    void initGraphicsDriver();

    static std::vector<const char *> getVulkanRequiredExtensions();

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    void setDimensions();

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_currentImage;
};
