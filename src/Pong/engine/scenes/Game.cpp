#include "Game.h"
#include <iostream>
#include "Scene.h"

Game* Game::m_instance = nullptr;

Game::Game(GLFWwindow* window, Renderer* renderer) : m_renderer(renderer), m_scenes(), m_currentScene(nullptr), m_window(window)
{
	Game::setInstance(this);
}

Game::~Game()
{
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

		while (sceneToRun == m_currentScene) {
			if (glfwWindowShouldClose(m_window))
				return;

			glfwPollEvents();
			m_currentScene->run();
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
	auto scene = new Scene(name, m_renderer);

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
	return GameProperties(m_renderer->getWidth(), m_renderer->getHeight());
}