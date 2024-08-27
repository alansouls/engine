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

#include "VulkanDriver.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
	void run() {
		initWindow();
		m_driver = new VulkanDriver(getRequiredExtensions(), validationLayers, deviceExtensions, window, GraphicsDriverOptions{ enableValidationLayers, readFile("D:\\personal\\cpp\\VulkanIntro\\VulkanIntro\\shaders\\compiled\\vert.spv"), readFile("D:\\personal\\cpp\\VulkanIntro\\VulkanIntro\\shaders\\compiled\\frag.spv")});
		m_driver->init();
		mainLoop();
		cleanup();
	}

private:
	void initWindow() {
		std::cout << "Initializing window..." << std::endl;
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
		std::cout << "Finshed!" << std::endl;
	}

	void initVulkan() {
	}

	void mainLoop() {
		std::cout << "Entering main loop..." << std::endl;
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
		std::cout << "Finshed!" << std::endl;
	}

	void cleanup() {
		std::cout << "Cleaning up resources..." << std::endl;

		m_driver->cleanup();

		glfwDestroyWindow(window);

		glfwTerminate();

		std::cout << "Finshed!" << std::endl;
	}

	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}


	GLFWwindow* window;
	GraphicsDriver* m_driver;
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