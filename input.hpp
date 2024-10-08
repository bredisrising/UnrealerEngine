#pragma once
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include "game.hpp"
#include <iostream>



class Input {
    public:
        static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
        
        static glm::vec2 getMouseMove();
        static glm::vec2 getMousePos();

    private:
        static float x;
        static float y;
        static float lastX;
        static float lastY;
        static float deltaX;
        static float deltaY;
        static bool firstMouse;
};