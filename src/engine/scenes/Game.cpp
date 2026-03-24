#include "Game.h"

#include "../input/InputManager.h"
#include "Scene.h"
#include "core/Messenger.h"
#include "imgui.h"
#include "scenes/GameObject.h"
#include "scenes/SceneCreator.h"
#include "scenes/SceneDefinitions.h"
#include "scripts/CSharpExecutionEngine.h"
#include "scripts/GameAssemblyInfo.h"
#include "scripts/components/ScriptComponent.h"

#include <chrono>
#include <memory>
#include <stdexcept>
#include <utility>

Game::Game(EngineWindow *window, std::string dotnetProjectPath, std::string dotnetProjectName)
    : m_messenger(nullptr), m_renderer(nullptr), m_dotnetProjectPath(std::move(dotnetProjectPath)),
      m_dotnetProjectName(std::move(dotnetProjectName)), m_currentScene(nullptr), m_window(window), m_paused(false),
      m_started(false), m_shouldRun(false), m_scriptExecutionEngine(nullptr), m_inputManager(nullptr)
{
    setInstance(this);

    // Initialize input manager first
    m_inputManager = std::make_unique<SSGE::InputManager>();

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
    m_gameAssemblyLoaded = false;
}

auto Game::run() -> void
{
    while (true)
    {
        if (m_sceneToLoad)
        {
            m_originalScene.reset();
            stop();
            m_currentScene.reset();
            m_renderer->resetSceneRenderers();
            SSGE::SceneCreator::CreateScene(this, m_sceneToLoad.value());
            m_sceneToLoad.reset();
        }

        SSGE::Scene *sceneToRun = m_currentScene.get();

        if (!m_shouldRun)
        {
            m_originalScene = SSGE::SceneDefinition::FromInstance(sceneToRun);
        }

        if (m_shouldRun && !m_started)
        {
            initForRun();
        }

        long long frames = 0;
        long long elapsed = 0;
        long long frameTime = 0;
        const long long targetTime =
            m_fpsCap.has_value() ? static_cast<long long>(1000000000.0 / m_fpsCap.value() * 0.95) : 0;

        sceneToRun->initForRun();

        while ((m_started || !m_shouldRun) && !m_sceneToLoad.has_value())
        {
            preRun();

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
            frames++;
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            frameTime += duration;
            elapsed += duration;
            if (elapsed >= 1000000000)
            {
                const double fps = static_cast<double>(frames) * static_cast<double>(elapsed) / 1000000000;
                // std::cout << "FPS: " << fps << std::endl;
                elapsed = 0;
                frames = 0;
            }
        }
    }
}

Game *Game::getInstance()
{
    return s_instance;
}

void Game::setInstance(Game *instance)
{
    if (s_instance != nullptr)
    {
        throw std::runtime_error("Game instance already set");
    }

    s_instance = instance;
}

SSGE::Scene *Game::addScene(const std::string &name)
{
    m_currentScene = std::make_unique<SSGE::Scene>(name, m_scriptExecutionEngine, m_inputManager.get());

    return m_currentScene.get();
}

SSGE::Scene *Game::getCurrentScene() const
{
    return m_currentScene.get();
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
    return m_inputManager.get();
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
    m_gameAssemblyLoaded = false;
    m_shouldRun = false;
    m_started = false;
    m_currentScene->initForRun();
    if (m_originalScene)
    {
        m_sceneToLoad = m_originalScene;
    }
}

auto Game::isStarted() const -> bool
{
    return m_shouldRun;
}

Game *Game::s_instance = nullptr;

auto Game::initForRun() -> void
{
    // TODO: configure game main assembly name
    if (!m_scriptExecutionEngine->loadGameAssembly("SSGEDotNet.Sample.dll"))
    {
        throw std::runtime_error("Failed to compile C# scripts for scene");
    }

    m_gameAssemblyLoaded = true;

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

    m_gameAssemblyInfo = info.value();

    if (!m_currentScene)
    {
        return;
    }

    for (SSGE::GameObject *gameObject : m_currentScene->gameObjects())
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

auto Game::isGameAssemblyLoaded() const -> bool
{
    return m_gameAssemblyLoaded;
}

auto Game::setGameInputEnabled(bool enabled) -> void
{
    m_gameInputEnabled = enabled;
}

auto Game::isGameInputEnabled() const -> bool
{
    return m_gameInputEnabled;
}

auto Game::gameAssemblyInfo() const -> const SSGE::GameAssemblyInfo &
{
    return m_gameAssemblyInfo;
}

auto Game::messenger() const -> SSGE::Messenger *
{
    return m_messenger.get();
}

// TODO: Move ImGui dependent code out of this class
// GLFW callback handlers following ImGui's recommended pattern
auto Game::keyCallback(int key, int scancode, int action, int mods) const -> void
{
    if (m_inputManager && m_gameInputEnabled)
        m_inputManager->updateKeyState(key, action);
}

auto Game::mouseButtonCallback(int button, int action, int mods) const -> void
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse && m_inputManager && m_gameInputEnabled)
        m_inputManager->updateMouseButtonState(button, action);
}

auto Game::cursorPositionCallback(double xpos, double ypos) const -> void
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse && m_inputManager && m_gameInputEnabled)
    {
        m_inputManager->updateMousePosition(xpos, ypos);
    }
}

auto Game::scrollCallback(double xoffset, double yoffset) const -> void
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse && m_inputManager && m_gameInputEnabled)
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

auto Game::setSceneToLoad(SSGE::SceneDefinition sceneDefinition) -> void
{
    m_sceneToLoad = std::move(sceneDefinition);
}

auto Game::onGameObjectComponentRemoved(SSGE::GameObject *gameObject, SSGE::Component *component) -> void
{
    if (!isGameAssemblyLoaded())
        return;

    auto scriptEngine = SSGE::CSharpExecutionEngine::Get();

    auto removeFunc = scriptEngine->getComponentEntryPointFunctions()[SSGE::CSharpExecutionEngine::Remove];

    struct
    {
        SSGE::GameObject *gameObject;
        const char *componentName;
        SSGE::Component *component;
    } removeParams{
        .gameObject = gameObject,
        .componentName = component->name().c_str(),
        .component = component,
    };

    if (removeFunc(&removeParams, sizeof(removeParams)))
    {
        throw std::runtime_error("Could not remove component from scripting model");
    }
}

auto Game::setMessenger(std::unique_ptr<SSGE::Messenger> messenger) -> void
{
    m_messenger = std::move(messenger);

    m_messenger->connect<SSGE::GameObject::ComponentRemovedMessage>(
        this, [this](const SSGE::GameObject::ComponentRemovedMessage &message) {
            onGameObjectComponentRemoved(message.gameObject, message.componentName);
        });
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
