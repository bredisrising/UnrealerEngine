#include "helper.hpp"

std::vector<glm::vec3> Helper::loadImage(const std::string& fileName) {
    std::ifstream image(fileName, std::ios::binary);

    uint32_t address = 0U;

    std::vector<glm::vec3> imageColors;
    imageColors.resize(47 * 47);

    // bitmap is little-endian
    image.seekg(10);
    address += image.get();
    address += image.get() << 1;
    address += image.get() << 2;
    address += image.get() << 3;

    image.seekg(address);
    for (int i = 0; i < 47 * 47; i++) {

        imageColors[i][2] += image.get() / 255.0;
        imageColors[i][1] += image.get() / 255.0;
        imageColors[i][0] += image.get() / 255.0;
        image.get();
        // std::cout << imageColors[i][0] << " ";
    }
    return imageColors;
}


std::vector<char> Helper::readFile(const std::string& fileName) {
    // ifstream stands of input-file stream
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file " + fileName);
    }

    size_t fileSize = (size_t)file.tellg();
    if (fileSize == 0) {
        return std::vector<char>();
    }

    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    
    return buffer;
}

VkShaderModule Helper::createShaderModule(const std::vector<char> bytes, VkDevice device) {
    VkShaderModuleCreateInfo moduleCreateInfo{};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.codeSize = bytes.size();
    moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(bytes.data());

    VkShaderModule module;
    
    std::cout << "Shader Module " << string_VkResult(vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &module)) << std::endl;
    return module;
}

uint32_t Helper::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties{};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties){
            return i;
        }
    }
}

void Helper::createBuffer (VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory &bufferMemory) {
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