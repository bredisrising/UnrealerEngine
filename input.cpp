#include "input.hpp"

void Input::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    if (Input::firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = true;
    }

    deltaX = xpos - lastX;
    deltaY = ypos - lastY;

    lastX = xpos;
    lastY = ypos;
}

glm::vec2 Input::getMouseMoveVector() {
    return glm::vec2{deltaX, deltaY};
}