#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <iostream>

#include <vector>
#include <string>
#include <fstream>

namespace Helper {
    std::vector<char> readFile(const std::string& fileName);
    VkShaderModule createShaderModule(const std::vector<char> bytes, VkDevice device);
    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory &bufferMemory);

    std::vector<std::vector<uint32_t>> loadImage(std::string& fileName);
    void saveBytes(std::vector<char> bytes, std::string& fileName);
}