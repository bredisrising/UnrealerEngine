#include "renderer.hpp"
#include "game.hpp"

int Renderer::numPipelines = 0;
API* Renderer::api = nullptr;
std::vector<VkBuffer> Renderer::buffers;
std::vector<VkPipeline> Renderer::pipelines;
std::vector<VkDeviceSize> Renderer::offsets;

void Renderer::createCircleRenderer() {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo {};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

    VkVertexInputAttributeDescription vertexPositionAttribute {};
    vertexPositionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexPositionAttribute.offset = offsetof(Circle, position);

    VkVertexInputAttributeDescription vertexColorAttribute {};
    vertexColorAttribute.location = 1;
    vertexColorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexColorAttribute.offset = offsetof(Circle, color);

    VkVertexInputAttributeDescription vertexRadiusAttribute;
    vertexRadiusAttribute.binding = 0;
    vertexRadiusAttribute.location = 2;
    vertexRadiusAttribute.format = VK_FORMAT_R32_SFLOAT;
    vertexRadiusAttribute.offset = offsetof(Circle, radius); 

    VkVertexInputAttributeDescription attributeDescriptions[] = {vertexPositionAttribute, vertexColorAttribute, vertexRadiusAttribute};

    VkVertexInputBindingDescription vertexBindingDescription{};
    vertexBindingDescription.binding = 0;
    vertexBindingDescription.stride = sizeof(Circle);
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
    vertexShader.module = Helper::createShaderModule(Helper::readFile("./shaders/compiled/vert.spv"), api->logicalDevice);
    vertexShader.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShader.pName = "main";

    VkPipelineShaderStageCreateInfo fragShader{};
    fragShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShader.module = Helper::createShaderModule(Helper::readFile("./shaders/compiled/frag.spv"), api->logicalDevice);
    fragShader.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShader.pName = "main";
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {vertexShader, fragShader};

    api->graphicsPipelineCreateInfo.stageCount = 2;
    api->graphicsPipelineCreateInfo.pStages = shaderStageCreateInfos;
    api->graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    api->graphicsPipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    api->graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;

    pipelines.resize(pipelines.size()+1);
    buffers.resize(buffers.size()+1);
    offsets.resize(offsets.size()+1);
    numPipelines += 1;

    std::cout << "Circle Pipeline " << string_VkResult(vkCreateGraphicsPipelines(api->logicalDevice, nullptr, 1, &api->graphicsPipelineCreateInfo, nullptr, &pipelines[pipelines.size()-1])) << std::endl;
}