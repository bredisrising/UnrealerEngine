#pragma once
//#define GLFW_INCLUDE_VULKAN
#include <vector>
#include "game.hpp"
#include "vulkan/vulkan.h"
#include "vulkan/vk_enum_string_helper.h"   
#include "GLFW/glfw3.h"
#include "helper.hpp"
#include "object.hpp"
#include "glm/glm.hpp"

static const int MAX_FRAMES_IN_FLIGHT = 3;

class API {
    public:
        VkDevice logicalDevice;

        API(GLFWwindow* window);
        void initializeVulkan();
        void drawframe(int numParticles, VkBuffer vBuffer, VkDeviceSize offset);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory &bufferMemory);
        void uploadDataToBuffer(VkBuffer& buffer, void* data);


    private:
        int currentFrame;

        GLFWwindow* window;
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain;
        VkRenderPass renderPass;
        VkPipeline graphicsPipeline;

        VkExtent2D swapchainExtent{};

        VkCommandPool commandPool;

        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        std::vector<VkFramebuffer> swapchainFramebuffers;
        
        std::vector<VkCommandBuffer> commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences; 

        void recordCommandBuffer(int numParticles, VkCommandBuffer commandBuffer, uint32_t index, VkBuffer vBuffer, VkDeviceSize offset);
        void createCommandPool(int graphicsFamilyIndex);
        void createCommandBuffers();
        void createSyncObjects();

};