#pragma once
#include <chrono>

struct GameProperties
{
    uint32_t width;
    uint32_t height;
    std::chrono::nanoseconds deltaTime;
};