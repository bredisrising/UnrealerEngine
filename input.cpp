#include "input.hpp"

float Input::x = 0.0f;
float Input::y = 0.0f;
float Input::lastX = 0.0f;
float Input::lastY = 0.0f;
float Input::deltaX = 0.0f;
float Input::deltaY = 0.0f;
bool Input::firstMouse = true;


void Input::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    if (Input::firstMouse) {
        lastX = xpos / WIDTH * 2.0f - 1.0f;
        lastY = ypos / HEIGHT * 2.0f - 1.0f;
        firstMouse = false;
    }

    x = xpos / WIDTH * 2.0f - 1.0f;
    y = ypos / HEIGHT * 2.0f - 1.0f;

    deltaX = x - lastX;
    deltaY = y - lastY;

    lastX = x;
    lastY = y;
    
    // std::cout << x << ":" << y << "  ";

}

glm::vec2 Input::getMousePos(){
    return glm::vec2(x, y);
}

glm::vec2 Input::getMouseMove() {
    return glm::vec2{deltaX, deltaY};
}