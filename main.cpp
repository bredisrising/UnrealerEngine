#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "api.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "input.hpp"
#include "object.hpp"
#include "fluid.hpp"
#include "renderer.hpp"
#include "game.hpp"

int main() {
    std::vector<float> fpsValues;

    GLFWwindow* window;

    if(!glfwInit()){
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Unrealer Engine", NULL, NULL);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetCursorPosCallback(window, Input::cursorPositionCallback);

    API* api;
    api = new API(window);
    api->initializeVulkan();

    Renderer::logicalDevice = api->logicalDevice;
    Renderer::physicalDevice = api->physicalDevice;
    
    // cube renderers
    Cube* data = Renderer::createCubeRenderer(api->graphicsPipelineCreateInfo);
    Renderer::renderers[0].numVertices = 36;
    data[0].position = {0.0, 0.0, 6.0};
    

    // circle renderers
    Renderer::createCircleRenderer(api->graphicsPipelineCreateInfo);
    std::cout << "Created Circle Renderer" << std::endl;
    Fluid fluid;

    VkDeviceMemory vertexBufferMemory;
    VkDeviceSize maxBufferSize = sizeof(Circle) * fluid.maxParticles;
    VkDeviceSize bufferSize = sizeof(Circle) * fluid.maxParticles;

    std::cout << "Circle Buffer Size " << maxBufferSize / 1000000.0  << "MB" << std::endl;

    api->createBuffer(maxBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        Renderer::renderers[1].vertexBuffer, vertexBufferMemory
    );
    Circle* mappedCircles;
    vkMapMemory(api->logicalDevice, vertexBufferMemory, 0, maxBufferSize, 0, (void**)&mappedCircles);
    fluid.map(mappedCircles);

    std::cout << "Created and Mapped Buffer" << std::endl;

    glfwMakeContextCurrent(window);

    double previousTime = glfwGetTime();
    double currentTime = glfwGetTime();
    double fps = 0;
    double lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        
        if (glfwGetTime() > lastFrame+(1.0/FPS)){
            lastFrame = glfwGetTime();
            glfwPollEvents();


            Input::y = glm::clamp(Input::y, (float)-M_PI/2, (float)M_PI/2);
            glm::mat4 rotmatting = glm::transpose(glm::eulerAngleXYZ(Input::y, -Input::x, 0.0f));
            Camera::rotationMatrix = glm::transpose(rotmatting);
            Camera::forward = rotmatting * glm::vec4(0.0, 0.0, 1.0, 0.0);
            Camera::right = rotmatting * glm::vec4(1.0, 0.0, 0.0, 0.0);
            Camera::up = rotmatting * glm::vec4(0.0, 1.0, 0.0, 0.0);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
                Camera::worldPosition += Camera::forward * 1.0f/240.0f * Camera::moveSpeed;
            } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                Camera::worldPosition -= Camera::forward * 1.0f/240.0f * Camera::moveSpeed;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
                Camera::worldPosition += Camera::right * 1.0f/240.0f * Camera::moveSpeed;
            } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
                Camera::worldPosition -= Camera::right * 1.0f/240.0f * Camera::moveSpeed;
            }


            fluid.step();
            Renderer::renderers[1].numVertices = fluid.numParticles;

            // VkDeviceSize bufferSize = sizeof(Circle) * fluid.numParticles;
            // memcpy(data, fluid.particleCircles.data(), (size_t) bufferSize);
            
            api->drawframe();

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

            glfwSetWindowTitle(window, ss.str().c_str());
            previousTime = currentTime;
        }
    }

    glfwTerminate();
    return 0;

}
