#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "api.hpp"
#include "engine.hpp"
#include "ecsmanager.hpp"
#include "input.hpp"
#include "renderables.hpp"
#include "renderer.hpp"
#include "game.hpp"

int main() {
    std::vector<float> fpsValues;

    if(!glfwInit()){
        return -1;
    }

    // this one is needed for VULKAN to initialize properly
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Unrealer Engine", NULL, NULL);

    Engine engine(window);

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetCursorPosCallback(engine.window, Input::cursorPositionCallback);
    glfwSetWindowPos(engine.window, 50, 50);
    


    glfwMakeContextCurrent(engine.window);

    double previousTime = glfwGetTime();
    double currentTime = glfwGetTime();
    double fps = 0;
    double lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(engine.window)) {
        // should thread this function -> glfwPollEvents();
        glfwPollEvents();
        if (glfwGetTime() > lastFrame+(1.0/FPS)){
            lastFrame = glfwGetTime();
            

            // Input::y = glm::clamp(Input::y, (float)-M_PI/2, (float)M_PI/2);
            // glm::mat4 rotmatting = glm::transpose(glm::eulerAngleXYZ(Input::y, -Input::x, 0.0f));
            // Camera::rotationMatrix = glm::transpose(rotmatting);
            // Camera::forward = rotmatting * glm::vec4(0.0, 0.0, 1.0, 0.0);
            // Camera::right = rotmatting * glm::vec4(1.0, 0.0, 0.0, 0.0);
            // Camera::up = rotmatting * glm::vec4(0.0, 1.0, 0.0, 0.0);

            // if (glfwGetKey(engine.window, GLFW_KEY_W) == GLFW_PRESS){
            //     Camera::worldPosition += Camera::forward * 1.0f/240.0f * Camera::moveSpeed;
            // } else if (glfwGetKey(engine.window, GLFW_KEY_S) == GLFW_PRESS) {
            //     Camera::worldPosition -= Camera::forward * 1.0f/240.0f * Camera::moveSpeed;
            // }
            // if (glfwGetKey(engine.window, GLFW_KEY_D) == GLFW_PRESS){
            //     Camera::worldPosition += Camera::right * 1.0f/240.0f * Camera::moveSpeed;
            // } else if (glfwGetKey(engine.window, GLFW_KEY_A) == GLFW_PRESS){
            //     Camera::worldPosition -= Camera::right * 1.0f/240.0f * Camera::moveSpeed;
            // }

            engine.ecsmanager->step(); 


            engine.api->drawframe();


            currentTime = glfwGetTime();
            fps = 1 / (currentTime - previousTime);

            fpsValues.push_back(fps);
            if (fpsValues.size() > 50) {
                fpsValues.erase(fpsValues.begin());
            }

            float averageFPS = std::accumulate(fpsValues.begin(), fpsValues.end(), 0) / fpsValues.size();

            Time::deltatime = currentTime - previousTime;

            
            std::stringstream ss;
            ss << "Unrealer Engine " << "FPS: " << fps << " Average FPS: " << averageFPS << " Num Particles: " << " Current Time: " << Time::currentTime();

            glfwSetWindowTitle(engine.window, ss.str().c_str());
            previousTime = currentTime;
        }
    }

    glfwTerminate();
    return 0;

}
