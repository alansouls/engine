#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <set>
#include <optional>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <fstream>
#include <chrono>
#include "engine/graphics/renderers/Renderer.h"
#include "engine/graphics/renderers/RectangleItem.h"
#include "game/PongGame.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
const bool debugModeOn = false;
#else
const bool debugModeOn = true;
#endif

class GameApplication {
public:
	void run() {
		initWindow();
		m_game = new PongGame(debugModeOn, window);
		mainLoop();
		cleanup();
	}

private:
	void initWindow() {
		std::cout << "Initializing window..." << std::endl;
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Pong", nullptr, nullptr);

		std::cout << "Finshed!" << std::endl;
	}

	void mainLoop() {
		std::cout << "Entering main loop..." << std::endl;

		m_game->setup();
		m_game->run();

		std::cout << "Finshed!" << std::endl;
	}

	void cleanup() {
		std::cout << "Cleaning up resources..." << std::endl;

		delete m_game;

		glfwDestroyWindow(window);

		glfwTerminate();

		std::cout << "Finshed!" << std::endl;
	}

	GLFWwindow* window;
	Game* m_game;
};

int main() {
	GameApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
