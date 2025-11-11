#include "Game.h"
#include "../collisions/CollisionManager.h"
#include "../input/InputManager.h"
#include "Scene.h"
#include "imgui.h"
#include <chrono>
#include <iostream>
#include <thread>

Game *Game::m_instance = nullptr;

Game::Game(EngineWindow *window, const RendererOptions &options)
    : m_renderer(nullptr), m_scenes(), m_currentScene(nullptr), m_window(window), m_paused(false),
      m_collisionManager(new CollisionManager()), m_scriptExecutionEngine(nullptr), m_inputManager(nullptr)
{
    // Set up GLFW callbacks
    glfwSetKeyCallback(window->getWindow(), keyCallback);
    glfwSetMouseButtonCallback(window->getWindow(), mouseButtonCallback);
    glfwSetCursorPosCallback(window->getWindow(), cursorPositionCallback);
    glfwSetScrollCallback(window->getWindow(), scrollCallback);

    setInstance(this);

    // Initialize input manager first
    m_inputManager = new SSGE::InputManager();

    // Initialize renderer
    m_renderer = new Renderer(window, options);

    m_scriptExecutionEngine = SSGE::CSharpExecutionEngine::GetOrInitialize("SSGEDotNet.Sample", m_dotnetProjectPath);
}

Game::~Game()
{
    // Clear GLFW callbacks
    glfwSetKeyCallback(m_window->getWindow(), nullptr);
    glfwSetMouseButtonCallback(m_window->getWindow(), nullptr);
    glfwSetCursorPosCallback(m_window->getWindow(), nullptr);
    glfwSetScrollCallback(m_window->getWindow(), nullptr);

    for (auto scene : m_scenes)
    {
        delete scene;
    }

    delete m_inputManager;
    delete m_renderer;
}

void Game::run()
{
    if (auto *inputState = const_cast<SSGE::InputState *>(&m_inputManager->getInputState()); inputState)
    {
        m_scriptExecutionEngine->setInputState(inputState);
    }

    while (true)
    {
        auto sceneToRun = m_currentScene;

        long long elapsed = 0;
        long long frameTime = 0;
        const long long targetTime =
            m_fpsCap.has_value() ? static_cast<long long>(1000000000.0 / m_fpsCap.value() * 0.95) : 0;

        while (sceneToRun == m_currentScene)
        {
            auto start = std::chrono::high_resolution_clock::now();

            // TODO abstract this to engine window

            auto glfwWindow = m_window->getWindow();

            if (glfwWindowShouldClose(glfwWindow))
                return;

            glfwPollEvents();

            auto end = std::chrono::high_resolution_clock::now();
            long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            frameTime += duration;
            elapsed += duration;

            if (frameTime < targetTime)
            {
                continue;
            }

            frameTime -= duration;
            elapsed -= duration;

            m_deltaTime = std::chrono::nanoseconds(frameTime);
            frameTime = 0;

            // Update input state at the beginning of each frame
            if (m_inputManager)
            {
                m_inputManager->update();
            }

            if (!m_paused)
            {
                m_currentScene->run();
            }
            m_renderer->render();
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            frameTime += duration;
            elapsed += duration;
            if (elapsed >= 1000000000)
            {
                elapsed = 0;
            }
        }
    }
}

Game *Game::getInstance()
{
    return m_instance;
}

void Game::setInstance(Game *instance)
{
    if (m_instance != nullptr)
    {
        std::runtime_error("Game instance already set");
    }

    m_instance = instance;
}

SSGE::Scene *Game::addScene(const std::string &name)
{
    auto scene = new SSGE::Scene(name, m_collisionManager, m_scriptExecutionEngine, m_inputManager);

    m_scenes.push_back(scene);
    return scene;
}

void Game::removeScene(const std::string &name)
{
    auto it =
        std::find_if(m_scenes.begin(), m_scenes.end(), [name](SSGE::Scene *scene) { return scene->getName() == name; });
    if (it != m_scenes.end())
    {
        delete *it;
        m_scenes.erase(it);
    }
}

void Game::setCurrentScene(const std::string &name)
{
    auto it =
        std::find_if(m_scenes.begin(), m_scenes.end(), [name](SSGE::Scene *scene) { return scene->getName() == name; });
    if (it != m_scenes.end())
    {
        m_currentScene = *it;
    }

    if (!m_scriptExecutionEngine->compile())
    {
        throw std::runtime_error("Failed to compile C# scripts for scene");
    }
}

SSGE::Scene *Game::getCurrentScene() const
{
    return m_currentScene;
}

GameProperties Game::getProperties() const
{
    return GameProperties{m_renderer->getSceneWidth(), m_renderer->getSceneHeight(), m_deltaTime};
}

void Game::pause()
{
    m_paused = true;
}

void Game::resume()
{
    m_paused = false;
}

bool Game::isPaused() const
{
    return m_paused;
}

auto Game::setFPSCap(const std::optional<uint16_t> &fpsCap) -> void
{
    m_fpsCap = fpsCap;
}

auto Game::getFPSCap() const -> const std::optional<uint16_t> &
{
    return m_fpsCap;
}

auto Game::getRenderer() -> Renderer &
{
    return *m_renderer;
}

auto Game::getInputManager() -> SSGE::InputManager *
{
    return m_inputManager;
}

void Game::onKeyPressed(int key)
{
}

void Game::onKeyReleased(int key)
{
}

void Game::onKeyDown(int key)
{
}

// GLFW callback handlers following ImGui's recommended pattern
void Game::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto game = Game::getInstance();
    if (!game)
        return;

    // (1) ALWAYS forward keyboard data to ImGui! This is automatic with default backends.
    ImGuiIO &io = ImGui::GetIO();

    // (2) ONLY forward keyboard data to game if ImGui doesn't want it
    if (game->m_inputManager)
    {
        game->m_inputManager->updateKeyState(key, action);
    }
}

void Game::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    const auto game = getInstance();
    if (!game)
        return;

    // (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends.
    ImGuiIO &io = ImGui::GetIO();

    // (2) ONLY forward mouse data to game if ImGui doesn't want it
    if (!io.WantCaptureMouse && game->m_inputManager)
    {
        game->m_inputManager->updateMouseButtonState(button, action);
    }
}

void Game::cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    auto game = Game::getInstance();
    if (!game)
        return;

    // (1) Mouse position is always forwarded to ImGui automatically

    // (2) ONLY forward mouse position to game if ImGui doesn't want it
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse && game->m_inputManager)
    {
        game->m_inputManager->updateMousePosition(xpos, ypos);
    }
}

void Game::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    auto game = Game::getInstance();
    if (!game)
        return;

    // (1) Scroll is always forwarded to ImGui automatically

    // (2) ONLY forward scroll to game if ImGui doesn't want it
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse && game->m_inputManager)
    {
        game->m_inputManager->updateMouseScroll(xoffset, yoffset);
    }
}
