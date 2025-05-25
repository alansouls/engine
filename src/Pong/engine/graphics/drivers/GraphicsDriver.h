#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

struct GraphicsDriverOptions
{
    bool debugModeOn;
    uint8_t *defaultVertexShader;
    size_t defaultVertexShaderSize;
    uint8_t *defaultFragmentShader;
    size_t defaultFragmentShaderSize;
    uint8_t *circleVertexShader;
    size_t circleVertexShaderSize;
    uint8_t *circleFragmentShader;
    size_t circleFragmentShaderSize;
};
ß

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
    enum ElementType
    {
        Quad,
        Circle
    };

    GraphicsDriver(GLFWwindow *window, const GraphicsDriverOptions &options)
        : m_window(window), m_options(options), m_framebufferResized(false)
    {
    }

    virtual void init() = 0;

    virtual void cleanup() = 0;

    virtual void drawFrame(const std::vector<GraphicsOperation *> &updateOperations) = 0;

    virtual void performOperation(GraphicsOperation *operation) = 0;

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
