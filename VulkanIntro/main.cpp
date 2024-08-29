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
#include "graphics/renderers/Renderer.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		m_renderer = new Renderer(window, { RendererOptions::Vulkan, enableValidationLayers, std::nullopt });
		mainLoop();
		cleanup();
	}

private:
	void initWindow() {
		std::cout << "Initializing window..." << std::endl;
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Application", nullptr, nullptr);

		std::cout << "Finshed!" << std::endl;
	}

	void mainLoop() {
		std::cout << "Entering main loop..." << std::endl;
		m_renderer->createRectangleItem({ -0.9f, -0.2f }, 0.7f, 0.5f, { 0.0f, 1.0f, 0.0f }, Renderer::CoordinatesType::Normalized);
		m_renderer->createRectangleItem({ -0.2f, 0.2f }, 0.7f, 0.5f, { 1.0f, 0.0f, 0.0f }, Renderer::CoordinatesType::Normalized);
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			m_renderer->render();
		}

		std::cout << "Finshed!" << std::endl;
	}

	void cleanup() {
		std::cout << "Cleaning up resources..." << std::endl;

		delete m_renderer;

		glfwDestroyWindow(window);

		glfwTerminate();

		std::cout << "Finshed!" << std::endl;
	}

	GLFWwindow* window;
	Renderer* m_renderer;
};

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}