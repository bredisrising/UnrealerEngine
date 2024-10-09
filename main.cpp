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

    glfwSetCursorPosCallback(window, Input::cursorPositionCallback);

    API* api;
    api = new API(window);
    api->initializeVulkan();
    Renderer::api = api;
    Renderer::createCircleRenderer();

    Fluid fluid;

    VkDeviceMemory vertexBufferMemory;
    VkDeviceSize maxBufferSize = sizeof(Circle) * fluid.maxParticles;
    VkDeviceSize bufferSize = sizeof(Circle) * fluid.maxParticles;

    std::cout << "Circle Buffer Size " << maxBufferSize / 1000000.0  << "MB" << std::endl;

    api->createBuffer(maxBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        Renderer::buffers[0], vertexBufferMemory
    );

    Circle* mappedCircles;
    vkMapMemory(api->logicalDevice, vertexBufferMemory, 0, maxBufferSize, 0, (void**)&mappedCircles);

    fluid.map(mappedCircles);

    glfwMakeContextCurrent(window);

    double previousTime = glfwGetTime();
    double currentTime = glfwGetTime();
    double fps = 0;
    double lastFrame = glfwGetTime();

    fluid.lastPhysicsTime = Time::currentTime();
    std::cout << "Initial lastPhysicsTime: " << fluid.lastPhysicsTime << std::endl;
    while (!glfwWindowShouldClose(window)) {
        
        if (glfwGetTime() > lastFrame+(1.0/FPS)){
            lastFrame = glfwGetTime();
            glfwPollEvents();

            fluid.step();

            // VkDeviceSize bufferSize = sizeof(Circle) * fluid.numParticles;
            // memcpy(data, fluid.particleCircles.data(), (size_t) bufferSize);
            
            api->drawframe(fluid.numParticles, Renderer::numPipelines, Renderer::pipelines, Renderer::buffers, Renderer::offsets);

            currentTime = glfwGetTime();
            fps = 1 / (currentTime - previousTime);

            fpsValues.push_back(fps);
            if (fpsValues.size() > 50) {
                fpsValues.erase(fpsValues.begin());
            }

            float averageFPS = std::accumulate(fpsValues.begin(), fpsValues.end(), 0) / fpsValues.size();


            Time::deltatime = currentTime - previousTime;

            std::stringstream ss;
            ss << "Unrealer Engine " << "FPS: " << fps << " Average FPS: " << averageFPS << " Num Particles: " << fluid.numParticles << " Current Time: " << Time::currentTime();



            glfwSetWindowTitle(window, ss.str().c_str());
            previousTime = currentTime;
        }
    }

    glfwTerminate();
    return 0;

}
