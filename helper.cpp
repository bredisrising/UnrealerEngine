#include "helper.hpp"

void Helper::loadImage(const std::string& fileName, Helper::Image& image) {
    std::ifstream imageStream(fileName, std::ios::binary);

    uint32_t address = 0U;

    // bitmap is little-endian
    imageStream.seekg(10);
    address += imageStream.get();
    address += imageStream.get() << 8;
    address += imageStream.get() << 16;
    address += imageStream.get() << 24;

    uint32_t sizeofheader = 0;
    imageStream.seekg(14);
    sizeofheader += imageStream.get();
    sizeofheader += imageStream.get()<<8;
    sizeofheader += imageStream.get()<<16;
    sizeofheader += imageStream.get()<<24;

    std::cout << "Header Size " << sizeofheader << std::endl; 


    int32_t width = 0;
    int32_t height = 0;
    imageStream.seekg(18);
    width += imageStream.get();
    width += imageStream.get() << 8;
    width += imageStream.get() << 16;
    width += imageStream.get() << 24;

    height += imageStream.get();
    height += imageStream.get() << 8;
    height += imageStream.get() << 16;
    height += imageStream.get() << 24;

    std::cout << "Image Dim " << width << " x " << height << std::endl;

    image.width = width;
    image.height = height;
    image.pixels.resize(width * height);

    imageStream.seekg(address);
    for (int i = 0; i < width * height; i++) {

        image.pixels[i][2] += imageStream.get() / 255.0;
        image.pixels[i][1] += imageStream.get() / 255.0;
        image.pixels[i][0] += imageStream.get() / 255.0;
        imageStream.get();
        // std::cout << imageColors[i][0] << " ";
    }

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