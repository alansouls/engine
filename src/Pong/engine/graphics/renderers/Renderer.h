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
    enum RendererType
    {
        Vulkan,
        RendererTypeCount
    };

    RendererType type;
    bool debugModeOn;
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

    int getWidth() const
    {
        return m_width;
    }

    int getHeight() const
    {
        return m_height;
    }

    void addItem(RendererItem *item);

  private:
    EngineWindow *m_window;
    RendererOptions m_options;

    GraphicsDriver *m_driver;

    UIRenderer *m_uiRenderer;
    SceneRenderer *m_sceneRenderer;

    void initGraphicsDriver();

    std::vector<const char *> getVulkanRequiredExtensions() const;

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    void setDimensions();

    uint32_t m_width;
    uint32_t m_height;
};
