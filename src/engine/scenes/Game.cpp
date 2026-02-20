#include "Game.h"

#include "../input/InputManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "imgui.h"
#include "scripts/GameAssemblyInfo.h"
#include "scripts/components/ScriptComponent.h"

#include <chrono>

Game::Game(EngineWindow *window, SSGE::Renderer *renderer, std::string dotnetProjectPath, std::string dotnetProjectName)
    : m_dotnetProjectPath(std::move(dotnetProjectPath)), m_dotnetProjectName(std::move(dotnetProjectName)),
      m_renderer(renderer), m_currentScene(nullptr), m_window(window), m_paused(false), m_started(false),
      m_shouldRun(false), m_scriptExecutionEngine(nullptr), m_inputManager(nullptr)
{
    setInstance(this);

    // Initialize input manager first
    m_inputManager = new SSGE::InputManager();

    m_scriptExecutionEngine = SSGE::CSharpExecutionEngine::GetOrInitialize();

    m_window->setKeyCallback(
        [this](int key, int scancode, int action, int mod) { this->keyCallback(key, scancode, action, mod); });
    m_window->setMouseButtonCallback(
        [this](int button, int action, int mods) { this->mouseButtonCallback(button, action, mods); });
    m_window->setCursorPositionCallback([this](double xpos, double ypos) { this->cursorPositionCallback(xpos, ypos); });
    m_window->setScrollCallback([this](double xoffset, double yoffset) { this->scrollCallback(xoffset, yoffset); });
}

Game::~Game()
{
    m_scriptExecutionEngine->unloadGameAssembly();

    for (auto scene : m_scenes)
    {
        delete scene;
    }

    delete m_inputManager;
}

void Game::run()
{
    while (true)
    {
        auto sceneToRun = m_currentScene;

        while (true)
        {
            if (m_shouldRun && !m_started)
            {
                updateGameScriptInfo();
                initForRun();
            }

            long long elapsed = 0;
            long long frameTime = 0;
            const long long targetTime =
                m_fpsCap.has_value() ? static_cast<long long>(1000000000.0 / m_fpsCap.value() * 0.95) : 0;

            sceneToRun->initForRun();

            while ((m_started || !m_shouldRun) && sceneToRun == m_currentScene)
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

                if (m_shouldRun && !m_paused)
                {
                    // Update input state at the beginning of each frame
                    m_inputManager->update();
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
}

Game *Game::getInstance()
{
    return m_instance;
}

void Game::setInstance(Game *instance)
{
    if (m_instance != nullptr)
    {
        throw std::runtime_error("Game instance already set");
    }

    m_instance = instance;
}

SSGE::Scene *Game::addScene(const std::string &name)
{
    auto scene = new SSGE::Scene(name, m_scriptExecutionEngine, m_inputManager);

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
}

SSGE::Scene *Game::getCurrentScene() const
{
    return m_currentScene;
}

auto Game::getProperties() const -> GameProperties
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

auto Game::getRenderer() const -> SSGE::Renderer &
{
    return *m_renderer;
}

auto Game::getInputManager() const -> SSGE::InputManager *
{
    return m_inputManager;
}

auto Game::start() -> void
{
    if (m_shouldRun)
        return;

    m_shouldRun = true;
    m_started = false;
}

auto Game::stop() -> void
{
    if (!m_shouldRun)
        return;

    m_scriptExecutionEngine->unloadGameAssembly();
    m_currentScene->initForRun();
    m_shouldRun = false;
    m_started = false;
}

auto Game::isStarted() const -> bool
{
    return m_shouldRun;
}

Game *Game::m_instance = nullptr;

auto Game::initForRun() -> void
{
    // TODO: configure game main assembly name
    if (!m_scriptExecutionEngine->loadGameAssembly("SSGEDotNet.Sample.dll"))
    {
        throw std::runtime_error("Failed to compile C# scripts for scene");
    }

    if (auto *inputState = const_cast<SSGE::InputState *>(&m_inputManager->getInputState()); inputState)
    {
        m_scriptExecutionEngine->setInputState(inputState);
    }

    m_started = true;
}

auto Game::updateGameScriptInfo() -> void
{
    // TODO: configure game main assembly name
    std::optional<SSGE::GameAssemblyInfo> info = m_scriptExecutionEngine->getGameAssemblyInfo("SSGEDotNet.Sample.dll");
    if (!info)
    {
        throw std::runtime_error("Failed to compile C# scripts for scene");
    }

    if (!m_currentScene)
    {
        return;
    }

    for (auto &gameObject : m_currentScene->gameObjects())
    {
        for (SSGE::ScriptComponentInfo &componentInfo : info->Components)
        {
            if (std::optional<SSGE::ScriptComponent *> component =
                    gameObject->getComponent<SSGE::ScriptComponent>(componentInfo.FullName);
                component)
            {
                component.value()->updateFields(componentInfo);
            }
        }
    }
}

// GLFW callback handlers following ImGui's recommended pattern
auto Game::keyCallback(int key, int scancode, int action, int mods) const -> void
{
    if (m_inputManager)
        m_inputManager->updateKeyState(key, action);
}

auto Game::mouseButtonCallback(int button, int action, int mods) const -> void
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse && m_inputManager)
        m_inputManager->updateMouseButtonState(button, action);
}

auto Game::cursorPositionCallback(double xpos, double ypos) const -> void
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse && m_inputManager)
    {
        m_inputManager->updateMousePosition(xpos, ypos);
    }
}

auto Game::scrollCallback(double xoffset, double yoffset) const -> void
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse && m_inputManager)
    {
        m_inputManager->updateMouseScroll(xoffset, yoffset);
    }
}

auto Game::getDotnetProjectPath() const -> const std::string &
{
    return m_dotnetProjectPath;
}

auto Game::getDotnetProjectName() const -> const std::string &
{
    return m_dotnetProjectName;
}

// C-style API for interop with C#
extern "C"
{
    SSGE_API auto Game_GetInstance() -> Game *
    {
        return Game::getInstance();
    }

    SSGE_API auto Game_GetProperties(Game *game) -> GameProperties
    {
        return game->getProperties();
    }
}
