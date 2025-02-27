#pragma once
#include "GLFW/glfw3.h"
#include "renderer.hpp"
#include "ecsmanager.hpp"
#include "api.hpp"


class Engine {
    public:
        Engine(GLFWwindow* window);
        GLFWwindow* window;
        API* api;
        Renderer* circleRenderer;
        Renderer* cubeRenderer;
        ECSManager* ecsmanager;
        
    private:
};