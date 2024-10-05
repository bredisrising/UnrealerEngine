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

    Fluid fluid(12000);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkDeviceSize maxBufferSize = sizeof(Circle) * fluid.maxParticles;
    VkDeviceSize bufferSize = sizeof(Circle) * fluid.numParticles;

    std::cout << "Circle Buffer Size " << maxBufferSize / 1000000.0  << "MB" << std::endl;

    api->createBuffer(maxBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBuffer, vertexBufferMemory
    );

    void* data;
    vkMapMemory(api->logicalDevice, vertexBufferMemory, 0, maxBufferSize, 0, &data);
    memcpy(data, fluid.particleCircles.data(), (size_t) bufferSize);
    // vkUnmapMemory(api->logicalDevice, vertexBufferMemory);

    glfwMakeContextCurrent(window);

    double previousTime = glfwGetTime();
    double currentTime = glfwGetTime();
    double fps = 0;

    fluid.lastPhysicsTime = Time::currentTime();
    std::cout << "Initial lastPhysicsTime: " << fluid.lastPhysicsTime << std::endl;
    while (!glfwWindowShouldClose(window)) {    
        glfwPollEvents();

        fluid.step();

        VkDeviceSize bufferSize = sizeof(Circle) * fluid.numParticles;
        memcpy(data, fluid.particleCircles.data(), (size_t) bufferSize);
        
        api->drawframe(fluid.numParticles, vertexBuffer, 0);

        currentTime = glfwGetTime();
        fps = 1 / (currentTime - previousTime);

        Time::deltatime = currentTime - previousTime;

        std::stringstream ss;
        ss << "Unrealer Engine " << "FPS: " << fps << " Num Particles: " << fluid.numParticles << " Current Time: " << Time::currentTime();

        glfwSetWindowTitle(window, ss.str().c_str());
        previousTime = currentTime;
    }

    glfwTerminate();
    return 0;

}
