#pragma once
#include <chrono>

struct GameProperties {
	int width;
	int height;
	std::chrono::nanoseconds deltaTime;
};