#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define GLFW_INCLUDE_VULKAN
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp> 
#include <glm/gtx/euler_angles.hpp>
#include <glfw/glfw3.h>
#include <random>

constexpr float HEIGHT = 950.f;
constexpr float WIDTH = 950.f;

constexpr float BOUNDX = WIDTH / HEIGHT;
constexpr float BOUNDY = 1.0f;

constexpr int FPS = 240;

class Camera {
    public:
        static glm::mat4 getViewMatrix();
        static glm::mat4 getProjMatrix();

        static glm::vec4 worldPosition;

        static glm::vec4 forward;
        static glm::vec4 up;
        static glm::vec4 right;

        static glm::mat4 rotationMatrix;

        static float moveSpeed;

        static float E;
    private:    

};

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


