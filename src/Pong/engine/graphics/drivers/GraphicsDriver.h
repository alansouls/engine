#pragma once
#include "../utils/Vertex.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <array>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

struct GraphicsDriverOptions
{
    bool debugModeOn;
    const uint8_t *defaultVertexShader;
    size_t defaultVertexShaderSize;
    const uint8_t *defaultFragmentShader;
    size_t defaultFragmentShaderSize;
    const uint8_t *circleVertexShader;
    size_t circleVertexShaderSize;
    const uint8_t *circleFragmentShader;
    size_t circleFragmentShaderSize;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct GraphicsOperation;
class GraphicsDriver
{
  public:
    virtual ~GraphicsDriver() = default;
    enum ElementType
    {
        Triangle,
        Quad,
        Circle
    };

    GraphicsDriver(GLFWwindow *window, const GraphicsDriverOptions &options)
        : m_window(window), m_options(options), m_framebufferResized(false)
    {
    }

    virtual void init() = 0;

    virtual void cleanup() = 0;

    virtual void drawFrame(uint32_t currentFrame, ImDrawData *drawData) = 0;

    virtual void waitIdle() = 0;

    void windowResized()
    {
        m_framebufferResized = true;
    }

    virtual glm::vec2 getWindowSize() const = 0;

  protected:
    GLFWwindow *m_window;
    GraphicsDriverOptions m_options;
    bool m_framebufferResized;

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto driver = reinterpret_cast<GraphicsDriver *>(glfwGetWindowUserPointer(window));
        driver->windowResized();
    }
};
