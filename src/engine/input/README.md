# Input System

The input system provides a clean and flexible way to handle keyboard and mouse input from GLFW.

## Components

### InputState

Represents the current state of all inputs (keyboard keys, mouse buttons, mouse position, and scroll).

**Key States:**

- `Pressed`: Key was just pressed this frame
- `Held`: Key is being held down (after being pressed for more than one frame)
- `Released`: Key is not currently pressed

**Query Methods:**

```cpp
// Keyboard
bool isKeyPressed(int key)      // True only on the frame the key is first pressed
bool isKeyHeld(int key)         // True while the key is held down (includes pressed frame)
bool isKeyReleased(int key)     // True when the key is not pressed
KeyState getKeyState(int key)   // Get the exact state

// Mouse Buttons
bool isMouseButtonPressed(MouseButton button)
bool isMouseButtonHeld(MouseButton button)
bool isMouseButtonReleased(MouseButton button)

// Mouse Position & Movement
const MousePosition& getMousePosition()  // Get current mouse position
MousePosition getMouseDelta()            // Get mouse movement since last frame

// Mouse Scroll
const MouseScroll& getMouseScroll()      // Get scroll offset for this frame
```

### InputManager

Manages input capture from GLFW and maintains the current `InputState`.

**Key Features:**

- Enable/disable input capture
- Automatic callback registration with GLFW
- Frame-based state management

**Usage:**

```cpp
// InputManager is created automatically by the Game class
auto* inputManager = game->getInputManager();

// Enable/disable input capture
inputManager->enableCapture();
inputManager->disableCapture();

// Check if capture is enabled
bool enabled = inputManager->isCaptureEnabled();

// Get the current input state
const InputState& state = inputManager->getInputState();
```

## Integration with Scene

The `InputManager` is automatically available in every `Scene`:

```cpp
void MyScene::run()
{
    // Get input state from the scene
    const auto& input = getInputState();
    
    // Check for key presses
    if (input.isKeyPressed(GLFW_KEY_SPACE))
    {
        // Space was just pressed this frame
        player->jump();
    }
    
    // Check for held keys
    if (input.isKeyHeld(GLFW_KEY_W))
    {
        // W is being held down
        player->moveForward();
    }
    
    // Mouse input
    if (input.isMouseButtonPressed(MouseButton::Left))
    {
        auto pos = input.getMousePosition();
        // Left mouse button was clicked at pos
    }
    
    // Mouse movement
    auto delta = input.getMouseDelta();
    if (delta.x != 0.0 || delta.y != 0.0)
    {
        // Mouse moved
        camera->rotate(delta.x, delta.y);
    }
    
    // Mouse scroll
    auto scroll = input.getMouseScroll();
    if (scroll.yOffset != 0.0)
    {
        // Mouse was scrolled
        camera->zoom(scroll.yOffset);
    }
}
```

## GLFW Key Codes

Use GLFW key constants for keyboard input:

- `GLFW_KEY_SPACE`, `GLFW_KEY_ENTER`, `GLFW_KEY_ESCAPE`
- `GLFW_KEY_A` through `GLFW_KEY_Z`
- `GLFW_KEY_0` through `GLFW_KEY_9`
- Arrow keys: `GLFW_KEY_UP`, `GLFW_KEY_DOWN`, `GLFW_KEY_LEFT`, `GLFW_KEY_RIGHT`
- And many more... (see GLFW documentation)

## Mouse Buttons

Use the `MouseButton` enum:

```cpp
enum class MouseButton
{
    Left = 0,
    Right = 1,
    Middle = 2,
    Button4 = 3,
    Button5 = 4
};
```

## Implementation Details

### Frame-based State Management

- Input states are updated at the beginning of each frame
- `Pressed` states transition to `Held` after one frame
- Scroll offsets are reset each frame (they represent per-frame deltas)

### Enable/Disable Capture

When input capture is disabled:

- Callbacks are still registered but ignored
- No state updates occur
- Useful for when UI or other systems need exclusive input access

## Example: Player Movement

```cpp
void PlayerController::update(const InputState& input)
{
    // Movement
    glm::vec2 movement(0.0f);
    
    if (input.isKeyHeld(GLFW_KEY_W))
        movement.y += 1.0f;
    if (input.isKeyHeld(GLFW_KEY_S))
        movement.y -= 1.0f;
    if (input.isKeyHeld(GLFW_KEY_A))
        movement.x -= 1.0f;
    if (input.isKeyHeld(GLFW_KEY_D))
        movement.x += 1.0f;
    
    if (movement.x != 0.0f || movement.y != 0.0f)
    {
        movement = glm::normalize(movement);
        player->move(movement * speed * deltaTime);
    }
    
    // Jump
    if (input.isKeyPressed(GLFW_KEY_SPACE) && player->isGrounded())
    {
        player->jump();
    }
    
    // Shoot
    if (input.isMouseButtonPressed(MouseButton::Left))
    {
        auto mousePos = input.getMousePosition();
        player->shoot(mousePos);
    }
}
```
