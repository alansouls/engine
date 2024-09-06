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
#include "graphics/renderers/RectangleItem.h"

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
		float originalWidth = m_renderer->getWidth();
		float originalHeight = m_renderer->getHeight();
		float width = originalWidth;
		float height = originalHeight;
        float racketWidth = 20.0f;
        float racketHeight = 0.15f * height;
        float middle = (height - racketHeight) / 2;
        auto leftRacket = new RectangleItem({ 10.0f, middle }, racketWidth, racketHeight, { 0.0f, 1.0f, 0.0f });
        float rightRacketPos = width - racketWidth - 10.0f;
        auto rightRacket = new RectangleItem({ rightRacketPos, middle }, 20.0f, racketHeight, { 1.0f, 0.0f, 0.0f });
        m_renderer->addItem(leftRacket);
        m_renderer->addItem(rightRacket);
        auto start = std::chrono::high_resolution_clock::now();
        float steps[2] = {-1.0f, 1.0f};
        int currentLeftStep = 0;
        int currentRightStep = 1;
        float topLimit = 5.0f;
        float bottomLimit = height - racketHeight - 5.0f;
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			m_renderer->render();
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
            if (duration > 5) {
                start = std::chrono::high_resolution_clock::now();
                if (leftRacket->getTopLeft().y + steps[currentLeftStep] > topLimit && leftRacket->getTopLeft().y + steps[currentLeftStep] < bottomLimit){
                    leftRacket->moveY(steps[currentLeftStep]);
                }
                else {
                    currentLeftStep = (currentLeftStep + 1) % 2;
                }
                if (rightRacket->getTopLeft().y + steps[currentRightStep] > topLimit && rightRacket->getTopLeft().y + steps[currentRightStep] < bottomLimit){
                    rightRacket->moveY(steps[currentRightStep]);
                }
                else {
                    currentRightStep = (currentRightStep + 1) % 2;
                }
            }
			if (m_renderer->getWidth() != width || m_renderer->getHeight() != height) {
				width = m_renderer->getWidth();
				height = m_renderer->getHeight();
				racketHeight = 0.15f * height;
				bottomLimit = height - racketHeight - 5.0f;
				middle = (height - racketHeight) / 2;
				rightRacket->setScale({ 1.0f, height / originalHeight, 1.0f });
				leftRacket->setScale({ 1.0f, height / originalHeight, 1.0f });
				rightRacketPos = width - racketWidth - 10.0f;
				rightRacket->moveXTo(rightRacketPos);
			}
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
