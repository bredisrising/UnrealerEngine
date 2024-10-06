#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <random>

constexpr float HEIGHT = 950.f;
constexpr float WIDTH = 950.f;

constexpr float BOUNDX = WIDTH / HEIGHT;
constexpr float BOUNDY = 1.0f;



class Random {
    public:
        static float getRandom();

    private:
        static std::random_device rd;
        static std::mt19937 gen;
        static std::uniform_real_distribution<> dis;
};

class Time {
    public:
        // static float currentTime;
        static float deltatime;
        static float currentTime();
};


