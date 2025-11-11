#include "InputManager.h"

namespace SSGE
{

InputManager::InputManager()
{
}

auto InputManager::getInputState() const -> const InputState &
{
    return m_inputState;
}

auto InputManager::update() -> void
{
    m_inputState.beginFrame();
}

auto InputManager::updateKeyState(int key, int action) -> void
{
    KeyState state = KeyState::Released;
    
    if (action == GLFW_PRESS)
    {
        state = KeyState::Pressed;
    }
    else if (action == GLFW_RELEASE)
    {
        state = KeyState::Released;
    }
    else if (action == GLFW_REPEAT)
    {
        state = KeyState::Held;
    }

    m_inputState.updateKeyState(key, state);
}

auto InputManager::updateMouseButtonState(int button, int action) -> void
{
    // Map GLFW mouse button to our MouseButton enum
    if (button < 0 || button >= static_cast<int>(MouseButton::Count))
        return;

    MouseButton mouseButton = static_cast<MouseButton>(button);
    KeyState state = (action == GLFW_PRESS) ? KeyState::Pressed : KeyState::Released;

    m_inputState.updateMouseButtonState(mouseButton, state);
}

auto InputManager::updateMousePosition(double xpos, double ypos) -> void
{
    m_inputState.updateMousePosition(xpos, ypos);
}

auto InputManager::updateMouseScroll(double xoffset, double yoffset) -> void
{
    m_inputState.updateMouseScroll(xoffset, yoffset);
}

} // namespace SSGE