#pragma once
#include "vulkan/vulkan.h"
#include "api.hpp"
#include "helper.hpp"
#include <vector>

class Renderer {
    public:
        static std::vector<VkPipeline> pipelines;
        static std::vector<VkBuffer> buffers;
        static std::vector<VkDeviceSize> offsets;
        static int numPipelines;
        static void createCircleRenderer();
        static void createTriangleRenderer();
        static API* api;

};