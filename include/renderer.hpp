#pragma once
#include "vulkan/vulkan.h"
#include "renderables.hpp"
#include "components.hpp"
#include "helper.hpp"
#include <vector>

class Renderer {
    public:
        static std::vector<Renderer> renderers;
        // eventually, have only one create function that takes in all the stuff needed
        static Renderer createRenderer();

        static Renderer* createCircleRenderer(VkDevice& logicalDevice, VkPhysicalDevice& physicalDevice, VkGraphicsPipelineCreateInfo& createInfo);
        static Renderer* createCubeRenderer(VkDevice& logicalDevice, VkPhysicalDevice& physicalDevice, VkGraphicsPipelineCreateInfo& createInfo);

        bool hasVertexBuffer;
        bool hasPushConstants;
        bool hasDescriptorSets;
        
        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;
        VkBuffer vertexBuffer;
        VkBuffer otherBuffer;
        VkDescriptorSet descriptorSet;
        uint32_t numVertices;
        VkDeviceSize offset;
};