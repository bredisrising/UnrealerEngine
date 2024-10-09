#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <iostream>

#include "glm/vec3.hpp"

#include <vector>
#include <string>
#include <fstream>


namespace Helper {
    struct Image {
        int32_t width;
        int32_t height;
        std::vector<glm::vec3> pixels; 
    };

    std::vector<char> readFile(const std::string& fileName);
    VkShaderModule createShaderModule(const std::vector<char> bytes, VkDevice device);
    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory &bufferMemory);

    void loadImage(const std::string& fileName, Image& image);
    void saveBytes(std::vector<char> bytes, std::string& fileName);
}