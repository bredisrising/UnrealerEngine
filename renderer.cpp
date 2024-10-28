#include "renderer.hpp"
#include "game.hpp"


std::vector<Renderer> Renderer::renderers;

Renderer* Renderer::createCubeRenderer(VkDevice& logicalDevice, VkPhysicalDevice& physicalDevice, VkGraphicsPipelineCreateInfo& createInfo) {
    renderers.resize(renderers.size()+1);
    Renderer* cubeRenderer = &renderers[renderers.size()-1];


    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo {};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;


    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
    // vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions;
    // vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationCreateInfo.lineWidth = 1.0;

    VkPipelineShaderStageCreateInfo vertexShader{};
    vertexShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShader.module = Helper::createShaderModule(Helper::readFile("./shaders/compiled/cubevert.spv"), logicalDevice);
    vertexShader.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShader.pName = "main";

    VkPipelineShaderStageCreateInfo fragShader{};
    fragShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShader.module = Helper::createShaderModule(Helper::readFile("./shaders/compiled/cubefrag.spv"), logicalDevice);
    fragShader.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShader.pName = "main";
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {vertexShader, fragShader};


    createInfo.stageCount = 2;
    createInfo.pStages = shaderStageCreateInfos;
    createInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    createInfo.pRasterizationState = &rasterizationCreateInfo;
    createInfo.pVertexInputState = &vertexInputStateCreateInfo;


    // Buffer Creation
    VkDeviceSize maxBufferSize = sizeof(Cube) * 1;
    VkDeviceMemory bufferMemory;
    Helper::createBuffer(
        logicalDevice, physicalDevice, maxBufferSize, 
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        cubeRenderer->otherBuffer, bufferMemory
    );

    // Descriptor Creation  
    VkDescriptorSetLayoutBinding layoutBinding {};
    layoutBinding.binding = 0;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo {};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.bindingCount = 1;
    layoutCreateInfo.pBindings = &layoutBinding;
    

    VkDescriptorSetLayout descriptorSetLayout;
    vkCreateDescriptorSetLayout(logicalDevice, &layoutCreateInfo, nullptr, &descriptorSetLayout);

    VkDescriptorPoolSize poolSize;
    poolSize.descriptorCount = 1;
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.pPoolSizes = &poolSize;
    descriptorPoolCreateInfo.poolSizeCount = 1;

    VkDescriptorPool descriptorPool;
    vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool);

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
    
    vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocateInfo, &cubeRenderer->descriptorSet);


    VkDescriptorBufferInfo bufferInfo {};
    bufferInfo.buffer = cubeRenderer->otherBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet write{};
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstBinding = 0;
    write.dstSet = cubeRenderer->descriptorSet;
    write.pBufferInfo = &bufferInfo;



    // push constants
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(glm::vec4) + sizeof(glm::mat4); 

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    
    vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &cubeRenderer->pipelineLayout);
    createInfo.layout = cubeRenderer->pipelineLayout;

    cubeRenderer->hasDescriptorSets = true;
    cubeRenderer->hasPushConstants = true;

    vkUpdateDescriptorSets(logicalDevice, 1, &write, 0, nullptr);

    std::cout << "Cube Pipeline " << string_VkResult(vkCreateGraphicsPipelines(logicalDevice, nullptr, 1, &createInfo, nullptr, &cubeRenderer->pipeline)) << std::endl;

    // Cube* brutha;
    // vkMapMemory(logicalDevice, bufferMemory, 0, maxBufferSize, 0, (void**)&brutha);

    return cubeRenderer;
}

Renderer* Renderer::createCircleRenderer(VkDevice& logicalDevice, VkPhysicalDevice& physicalDevice, VkGraphicsPipelineCreateInfo& createInfo) {
    renderers.resize(renderers.size()+1);
    Renderer* circleRenderer = &renderers[renderers.size()-1];


    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo {};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

    // should generalize this? to the transforms
    VkVertexInputAttributeDescription vertexPositionAttribute {};
    vertexPositionAttribute.binding = 0;
    vertexPositionAttribute.location = 0;
    vertexPositionAttribute.format = VK_FORMAT_R32G32_SFLOAT;
    vertexPositionAttribute.offset = offsetof(Transform2D, position);

    VkVertexInputAttributeDescription vertexColorAttribute {};
    vertexColorAttribute.binding = 0;
    vertexColorAttribute.location = 1;
    vertexColorAttribute.format = VK_FORMAT_R32_SFLOAT;
    vertexColorAttribute.offset = offsetof(Transform2D, rotation);

    VkVertexInputAttributeDescription vertexRadiusAttribute;
    vertexRadiusAttribute.binding = 0;
    vertexRadiusAttribute.location = 2;
    vertexRadiusAttribute.format = VK_FORMAT_R32_SFLOAT;
    vertexRadiusAttribute.offset = offsetof(Transform2D, scale); 

    VkVertexInputAttributeDescription attributeDescriptions[] = {vertexPositionAttribute, vertexColorAttribute, vertexRadiusAttribute};

    VkVertexInputBindingDescription vertexBindingDescription{};
    vertexBindingDescription.binding = 0;
    vertexBindingDescription.stride = sizeof(Transform2D);
    vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 3;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationCreateInfo.lineWidth = 1.0;

    VkPipelineShaderStageCreateInfo vertexShader{};
    vertexShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShader.module = Helper::createShaderModule(Helper::readFile("./shaders/compiled/vert.spv"), logicalDevice);
    vertexShader.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShader.pName = "main";

    VkPipelineShaderStageCreateInfo fragShader{};
    fragShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShader.module = Helper::createShaderModule(Helper::readFile("./shaders/compiled/frag.spv"), logicalDevice);
    fragShader.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShader.pName = "main";
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {vertexShader, fragShader};

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // Renderer::renderers[Renderer::renderers.size()-1].pipelineLayout = {};
    vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &circleRenderer->pipelineLayout);
    

    createInfo.stageCount = 2;
    createInfo.pStages = shaderStageCreateInfos;
    createInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    createInfo.pRasterizationState = &rasterizationCreateInfo;
    createInfo.pVertexInputState = &vertexInputStateCreateInfo;
    createInfo.layout = circleRenderer->pipelineLayout;


    circleRenderer->hasVertexBuffer = true;

    std::cout << "Circle Pipeline " << string_VkResult(vkCreateGraphicsPipelines(logicalDevice, nullptr, 1, &createInfo, nullptr, &circleRenderer->pipeline)) << std::endl;

    return circleRenderer;

}