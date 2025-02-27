#include "input.hpp"

float Input::x = 0.0f;
float Input::y = 0.0f;
float Input::lastX = 0.0f;
float Input::lastY = 0.0f;
float Input::realX = 0.0f;
float Input::realY = 0.0f;
float Input::deltaX = 0.0f;
float Input::deltaY = 0.0f;
bool Input::firstMouse = true;

// void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
// {
//     if (key == GLFW_KEY_W && action == GLFW_PRESS)
//         activate_airship();
// }

void Input::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    if (Input::firstMouse) {
        lastX = xpos / WIDTH * 2.0f - 1.0f;
        lastY = ypos / HEIGHT * 2.0f - 1.0f;
        firstMouse = false;
    }


    realX = xpos / WIDTH * 2.0f - 1.0f;
    realY = ypos / HEIGHT * 2.0f - 1.0f;

    deltaX = realX - lastX;
    deltaY = realY - lastY;

    x += deltaX;
    y += deltaY;

    lastX = realX;
    lastY = realY;
}

glm::vec2 Input::getCursorPos(){
    return glm::vec2(realX*ASPECT_RATIO, realY);
}

glm::vec2 Input::getMousePos(){
    // std::cout << x << ":" << y << " ";
    return glm::vec2(x, y);
}

glm::vec2 Input::getMouseMove() {
    return glm::vec2{deltaX, deltaY};
}