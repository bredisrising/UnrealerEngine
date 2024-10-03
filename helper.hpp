#pragma once


#include <vulkan/vulkan.h>

#include <iostream>

#include <vector>
#include <string>
#include <fstream>

namespace Helper {
    inline std::vector<char> readFile(const std::string& fileName){
        // ifstream stands of input-file stream
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        

        return buffer;
    }

    inline VkShaderModule createShaderModule(const std::vector<char> bytes, VkDevice device){
        VkShaderModuleCreateInfo moduleCreateInfo{};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = bytes.size();
        moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(bytes.data());

        VkShaderModule module;
        
        std::cout << "Shader Module " << string_VkResult(vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &module)) << std::endl;
        return module;
    }

    inline uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties{};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties){
                return i;
            }
        }
    }

    inline void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory &bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

        vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);

        vkBindBufferMemory(device, buffer, bufferMemory, 0);    

    }

}