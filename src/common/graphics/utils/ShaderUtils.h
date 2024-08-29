#pragma once
#include <vector>
#include <fstream>

class ShaderUtils {
public:
	static std::vector<char> loadShader(const std::string& filename, bool compiled = true) {
		const char* basePath = compiled ? compiledShaderPath : shaderPath;
		std::string shaderPath = std::string(basePath) + filename;
		return readFile(shaderPath);
	}

private:
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

	static inline const char* compiledShaderPath = "..\\..\\common\\graphics\\drivers\\shaders\\compiled\\";
	static inline const char* shaderPath = "..\\..\\common\\graphics\\drivers\\shaders\\";
};