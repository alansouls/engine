#pragma once

#include "../drivers/GraphicsOperation.h"
#include "../utils/Vertex.h"
#include <engine/graphics/EngineWindow.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <map>
#include <set>
#include "ui/UIRenderer.h"

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

    std::map<uint32_t, RendererItem *> m_items;

    std::set<RendererItem *> m_addedSet;
    std::set<uint32_t> m_removedSet;
    std::set<uint32_t> m_updatedSet;

    void initGraphicsDriver();

    std::vector<const char *> getVulkanRequiredExtensions() const;

    std::map<RendererItem *, GraphicsOperation> getAddOrRemoveOperations();
    std::vector<GraphicsOperation> getUpdateOperations();

    void setDimensions();

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    uint32_t m_width;
    uint32_t m_height;

    friend class RendererItem;

    static void itemUpdated(void *thisPtr, uint32_t itemKey);

    glm::vec3 getResizeScale(float width, float height) const;
};
