#include "InputState.h"

namespace SSGE
{

auto InputState::isKeyPressed(int key) const -> bool
{
    auto it = m_keyStates.find(key);
    return it != m_keyStates.end() && it->second == KeyState::Pressed;
}

auto InputState::isKeyReleased(int key) const -> bool
{
    auto it = m_keyStates.find(key);
    return it == m_keyStates.end() || it->second == KeyState::Released;
}

auto InputState::isKeyHeld(int key) const -> bool
{
    auto it = m_keyStates.find(key);
    return it != m_keyStates.end() && (it->second == KeyState::Held || it->second == KeyState::Pressed);
}

auto InputState::getKeyState(int key) const -> KeyState
{
    auto it = m_keyStates.find(key);
    return it != m_keyStates.end() ? it->second : KeyState::Released;
}

auto InputState::isMouseButtonPressed(MouseButton button) const -> bool
{
    return m_mouseButtonStates[static_cast<size_t>(button)] == KeyState::Pressed;
}

auto InputState::isMouseButtonReleased(MouseButton button) const -> bool
{
    return m_mouseButtonStates[static_cast<size_t>(button)] == KeyState::Released;
}

auto InputState::isMouseButtonHeld(MouseButton button) const -> bool
{
    auto state = m_mouseButtonStates[static_cast<size_t>(button)];
    return state == KeyState::Held || state == KeyState::Pressed;
}

auto InputState::getMousePosition() const -> const MousePosition &
{
    return m_mousePosition;
}

auto InputState::getMouseDelta() const -> MousePosition
{
    return {
        m_mousePosition.x - m_previousMousePosition.x,
        m_mousePosition.y - m_previousMousePosition.y
    };
}

auto InputState::getMouseScroll() const -> const MouseScroll &
{
    return m_mouseScroll;
}

auto InputState::updateKeyState(int key, KeyState state) -> void
{
    m_keyStates[key] = state;
}

auto InputState::updateMouseButtonState(MouseButton button, KeyState state) -> void
{
    m_mouseButtonStates[static_cast<size_t>(button)] = state;
}

auto InputState::updateMousePosition(double x, double y) -> void
{
    m_previousMousePosition = m_mousePosition;
    m_mousePosition.x = x;
    m_mousePosition.y = y;
}

auto InputState::updateMouseScroll(double xOffset, double yOffset) -> void
{
    m_mouseScroll.xOffset = xOffset;
    m_mouseScroll.yOffset = yOffset;
}

auto InputState::beginFrame() -> void
{
    // Transition Pressed -> Held
    for (auto &[key, state] : m_keyStates)
    {
        if (state == KeyState::Pressed)
        {
            state = KeyState::Held;
        }
    }

    // Transition mouse button Pressed -> Held
    for (auto &state : m_mouseButtonStates)
    {
        if (state == KeyState::Pressed)
        {
            state = KeyState::Held;
        }
    }

    // Reset scroll delta (scroll is per-frame)
    m_mouseScroll.xOffset = 0.0;
    m_mouseScroll.yOffset = 0.0;
}

} // namespace SSGE
