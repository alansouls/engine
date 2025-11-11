#pragma once

#include "InputState.h"
#include <GLFW/glfw3.h>
#include <memory>

namespace SSGE
{

class InputManager
{
  public:
    InputManager();
    ~InputManager() = default;

    // Disable copy and move
    InputManager(const InputManager &) = delete;
    InputManager &operator=(const InputManager &) = delete;
    InputManager(InputManager &&) = delete;
    InputManager &operator=(InputManager &&) = delete;

    // Get the current input state
    [[nodiscard]] auto getInputState() const -> const InputState &;

    // Update input state - should be called at the beginning of each frame
    auto update() -> void;

    // Public methods to update input state (called from Game class)
    auto updateKeyState(int key, int action) -> void;
    auto updateMouseButtonState(int button, int action) -> void;
    auto updateMousePosition(double xpos, double ypos) -> void;
    auto updateMouseScroll(double xoffset, double yoffset) -> void;

  private:
    InputState m_inputState;
};

} // namespace SSGE
