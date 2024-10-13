#pragma once
#include "vulkan/vulkan.h"
#include "helper.hpp"
#include "object.hpp"
#include <vector>

class Renderer {
    public:
        static std::vector<Renderer> renderers;
        static void createCircleRenderer(VkGraphicsPipelineCreateInfo& createInfo);
        static Cube* createCubeRenderer(VkGraphicsPipelineCreateInfo& createInfo);
        static VkDevice logicalDevice;
        static VkPhysicalDevice physicalDevice;

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