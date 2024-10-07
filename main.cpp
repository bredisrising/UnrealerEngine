#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "api.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "object.hpp"
#include "fluid.hpp"
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

    //glfwSetCursorPosCallback(window, Input::cursorPositionCallback);

    API* api;
    api = new API(window);
    api->initializeVulkan();
    
    // int numFluidParticles;
    // std::cout << "Num Fluid Particles: ";
    // std::cin >> numFluidParticles;
    // std::cout << std::endl;

    int maxParticles = 50000;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkDeviceSize maxBufferSize = sizeof(Circle) * maxParticles;
    VkDeviceSize bufferSize = sizeof(Circle) * maxParticles;

    std::cout << "Circle Buffer Size " << maxBufferSize / 1000000.0  << "MB" << std::endl;

    api->createBuffer(maxBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBuffer, vertexBufferMemory
    );

    Circle* mappedCircles;
    vkMapMemory(api->logicalDevice, vertexBufferMemory, 0, maxBufferSize, 0, (void**)&mappedCircles);

    Fluid fluid(mappedCircles, maxParticles);

    glfwMakeContextCurrent(window);

    double previousTime = glfwGetTime();
    double currentTime = glfwGetTime();
    double fps = 0;

    double maxFPS = 240;
    double lastFrame = glfwGetTime();

    fluid.lastPhysicsTime = Time::currentTime();
    std::cout << "Initial lastPhysicsTime: " << fluid.lastPhysicsTime << std::endl;
    while (!glfwWindowShouldClose(window)) {
        
        if (glfwGetTime() > lastFrame+(1.0/maxFPS)){
        lastFrame = glfwGetTime();
        glfwPollEvents();

        fluid.step();

        // VkDeviceSize bufferSize = sizeof(Circle) * fluid.numParticles;
        // memcpy(data, fluid.particleCircles.data(), (size_t) bufferSize);
        
        api->drawframe(fluid.numParticles, vertexBuffer, 0);

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
