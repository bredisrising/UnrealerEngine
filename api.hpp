#pragma once

#include <vector>
#include "game.hpp"
#include "vulkan/vulkan.h"
#include "vulkan/vk_enum_string_helper.h"   
#include "GLFW/glfw3.h"
#include "helper.hpp"
#include "object.hpp"
#include "renderer.hpp"
#include "glm/glm.hpp"

static const int MAX_FRAMES_IN_FLIGHT = 3;

class API {
    public:
        VkDevice logicalDevice;
        VkPhysicalDevice physicalDevice;

        API(GLFWwindow* window);
        void initializeVulkan();
        void drawframe();
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory &bufferMemory);
        void uploadDataToBuffer(VkBuffer& buffer, void* data);

        void initVulkan();
        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createImageViews();
        void createSwapChain();
        void createRenderPass();
        void createFramebuffers();

        VkExtent2D swapchainExtent;
        VkViewport viewport;
        VkRect2D scissor;
        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlending;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineLayoutCreateInfo layoutCreateInfo;
        


    private:
        int currentFrame;

        GLFWwindow* window;
        VkInstance instance;

        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain;
        VkRenderPass renderPass;


        VkCommandPool commandPool;

        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        std::vector<VkFramebuffer> swapchainFramebuffers;
        
        std::vector<VkCommandBuffer> commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences; 

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);
        void createCommandPool(int graphicsFamilyIndex);
        void createCommandBuffers();
        void createSyncObjects();

};
