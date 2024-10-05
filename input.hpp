#pragma once
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

class Input {
    public:
        static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
        
        static glm::vec2 getMouseMoveVector();

    private:
        static float lastX;
        static float lastY;
        static float deltaX;
        static float deltaY;
        static bool firstMouse;
};