#include "Game.h"
#include <iostream>
#include "Scene.h"
#include "../collisions/CollisionManager.h"
#include <chrono>

Game* Game::m_instance = nullptr;

Game::Game(GLFWwindow* window, Renderer* renderer) : m_renderer(renderer), m_scenes(), m_currentScene(nullptr), m_window(window), m_paused(false), m_collisionManager(new CollisionManager())
{
	Game::setInstance(this);

	glfwSetKeyCallback(window, keyCallback);
}

Game::~Game()
{
	glfwSetKeyCallback(m_window, nullptr);

	for (auto scene : m_scenes)
	{
		delete scene;
	}

	delete m_renderer;
}

void Game::run()
{
	while (m_currentScene) {

		auto sceneToRun = m_currentScene;

		long long elasped = 0;
		while (sceneToRun == m_currentScene) {
			auto start = std::chrono::high_resolution_clock::now();
			if (glfwWindowShouldClose(m_window))
				return;

			glfwPollEvents();

			if (!m_paused) {
				m_currentScene->run();
				auto end = std::chrono::high_resolution_clock::now();
				long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
				elasped += duration;
				if (elasped >= 1000000000.0) {
					std::cout << "FPS: " << (1.0 / duration) * 1000000000.0 << " TIME: " << duration << " ns\n";
					elasped = 0;
				}
			}
		}
	}
}

Game* Game::getInstance()
{
	return m_instance;
}

void Game::setInstance(Game* instance)
{
	if (m_instance != nullptr) {
		std::runtime_error("Game instance already set");
	}
	m_instance = instance;
}

Scene* Game::addScene(const std::string& name)
{
	auto scene = new Scene(name, m_renderer, m_collisionManager);

	m_scenes.push_back(scene);
	return scene;
}

void Game::removeScene(const std::string& name)
{
	auto it = std::find_if(m_scenes.begin(), m_scenes.end(), [name](Scene* scene) { return scene->getName() == name; });
	if (it != m_scenes.end())
	{
		delete* it;
		m_scenes.erase(it);
	}
}

void Game::setCurrentScene(const std::string& name)
{
	auto it = std::find_if(m_scenes.begin(), m_scenes.end(), [name](Scene* scene) { return scene->getName() == name; });
	if (it != m_scenes.end())
	{
		m_currentScene = *it;
	}
}

Scene* Game::getCurrentScene() const
{
	return m_currentScene;
}

GameProperties Game::getProperties() const
{
    return GameProperties{m_renderer->getWidth(), m_renderer->getHeight()};
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

void Game::onKeyPressed(int key)
{
}

void Game::onKeyReleased(int key)
{
}

void Game::onKeyDown(int key)
{
}

void Game::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto game = Game::getInstance();

	auto scene = game->getCurrentScene();

	if (!scene)
		return;

	if (action == GLFW_PRESS)
	{
		game->onKeyPressed(key);
		game->m_currentScene->onKeyPressed(key);
	}
	else if (action == GLFW_RELEASE)
	{
		game->onKeyReleased(key);
		game->m_currentScene->onKeyReleased(key);
	}
	else if (action == GLFW_REPEAT)
	{
		game->onKeyDown(key);
		game->m_currentScene->onKeyDown(key);
	}
}
