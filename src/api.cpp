#include "api.hpp"
#include <iostream>
#include <vector>


API::API(GLFWwindow* window): window(window), currentFrame(0) {

}

void API::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory &bufferMemory) {
    Helper::createBuffer(logicalDevice, physicalDevice, size, usage, properties, buffer, bufferMemory);
}

void API::initializeVulkan() {

    // check available layers
    uint32_t numLayers;
    vkEnumerateInstanceLayerProperties(&numLayers, nullptr);
    VkLayerProperties* layerProperties = new VkLayerProperties[numLayers];
    vkEnumerateInstanceLayerProperties(&numLayers, layerProperties);
    for (int i = 0; i < numLayers; i++) {
        std::cout << layerProperties[i].layerName << std::endl;
    }
    std::cout << std::endl;
    
   
    const std::vector<const char*> enabledLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> enabledInstanceExtensions = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = enabledLayers.data();
    createInfo.enabledExtensionCount = enabledInstanceExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledInstanceExtensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
        std::cout << "instance creation failed!" << std::endl;
    }

    std::cout << "Surface Creation: " << string_VkResult(glfwCreateWindowSurface(instance, window, nullptr, &surface)) << std::endl << std::endl;


    uint32_t numPhysicalDevices;
    vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr);
    VkPhysicalDevice* devices = new VkPhysicalDevice[numPhysicalDevices];    
    vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, devices);
    std::cout << "Num Physical Devices: " << numPhysicalDevices << std::endl;

    for (int deviceIndex = 0; deviceIndex < numPhysicalDevices; deviceIndex++) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(devices[deviceIndex], &deviceProperties);

        std::cout << deviceProperties.deviceName << std::endl;

        if (deviceProperties.deviceName[0] == 'N') {
            physicalDevice = devices[deviceIndex];
        }
    }
    delete[] devices;
    devices = nullptr;

    
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    std::cout << "Chosen: " << deviceProperties.deviceName << "\n" << std::endl;
    
    
    //  
    uint32_t numFamilies;
    
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numFamilies, nullptr);
    VkQueueFamilyProperties* familyProperties = new VkQueueFamilyProperties[numFamilies];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numFamilies, familyProperties);

    uint32_t graphicsQueueIndex;
    uint32_t presentQueueIndex;
    uint32_t queueCount;

    for (int i = 0; i < numFamilies; i++){
        std::cout << i << " " << std::to_string(familyProperties[i].queueCount) << " " << std::to_string(familyProperties[i].queueFlags);
        if (familyProperties[i].queueFlags & 1) {
            std::cout << " has graphics capability";
            graphicsQueueIndex = i;
        }

        VkBool32 presentSupport;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (presentSupport == VK_TRUE){
            std::cout << " has present capability";
            presentQueueIndex = i;
        }
        
        std::cout << std::endl;
    }
    std::cout << std::endl;

    graphicsQueueIndex = 0;
    presentQueueIndex = 0;


    //device queues
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo graphicsQueueCI{};
    graphicsQueueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCI.queueFamilyIndex = graphicsQueueIndex;
    graphicsQueueCI.queueCount = 1;
    graphicsQueueCI.pQueuePriorities = &queuePriority;

    VkDeviceQueueCreateInfo queueCreateInfos[] = {graphicsQueueCI};

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };



    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);

    vkGetDeviceQueue(logicalDevice, graphicsQueueIndex, 0, &graphicsQueue);



    //swapchain
    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = 3; // triple buffering
    swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    
    swapchainExtent.height = HEIGHT;
    swapchainExtent.width = WIDTH;

    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR; // triple buffering
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR; // does nothing
    swapchainCreateInfo.clipped = VK_TRUE;
    
    uint32_t queueFamilyIndices[] = {graphicsQueueIndex};
    swapchainCreateInfo.queueFamilyIndexCount = 1;
    swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;


    vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain);

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapchainImages.data());

    std::cout << "Swapchain Image Count: " << imageCount << std::endl;

    swapchainImageViews.resize(imageCount);

    for (int image = 0; image < swapchainImages.size(); image++){
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = swapchainImages[image];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(logicalDevice, &imageViewCreateInfo, nullptr, &swapchainImageViews[image]);
    }

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;

    std::cout << "Render Pass Creation " << string_VkResult(vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, nullptr, &renderPass)) << std::endl;



    // graphics pipeline
    viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = WIDTH;
    viewport.height = HEIGHT;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor = {};
    scissor.extent = swapchainExtent;
    scissor.offset = {0, 0};


    viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    rasterizationCreateInfo = {};
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationCreateInfo.lineWidth = 1.0;

    colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;



    // minimal graphics pipeline
    // input assembly
    // vertex shader
    // viewport/scissor states
    // rasterization
    // fragment shader
    // depth/stencil

    graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlending;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampling;
    graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
    graphicsPipelineCreateInfo.renderPass = renderPass;
    graphicsPipelineCreateInfo.subpass = 0;

    std::cout << "swapchainImageViews size: " << swapchainImageViews.size() << std::endl;
    std::cout << "swapchainExtent width: " << swapchainExtent.width << ", height: " << swapchainExtent.height << std::endl;
    std::cout << "logicalDevice: " << logicalDevice << std::endl;
    std::cout << "renderPass: " << renderPass << std::endl;

    swapchainFramebuffers.resize(swapchainImageViews.size());

    for (size_t i = 0; i < swapchainImageViews.size(); i++){
        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = &swapchainImageViews[i];
        framebufferCreateInfo.width = swapchainExtent.width;
        framebufferCreateInfo.height = swapchainExtent.height;
        framebufferCreateInfo.layers = 1;
        
        std::cout << i << " Framebuffer Creation: " << string_VkResult(vkCreateFramebuffer(logicalDevice, &framebufferCreateInfo, nullptr, &swapchainFramebuffers[i])) << std::endl << std::endl;

    }

    createCommandPool(graphicsQueueIndex);
    createCommandBuffers();

    createSyncObjects();
}

void API::drawframe() {
    //std::cout << "Current Frame " << currentFrame << std::endl;
    vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSemaphore waitSemaphore[] = {imageAvailableSemaphores[currentFrame]};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphore[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphore;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);

    
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphore;

    VkSwapchainKHR swapchains[] = {swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(graphicsQueue, &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}



void API::createSyncObjects(){
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
        vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
        vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]);
    }

}

void API::createCommandBuffers(){
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if(vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS){
        std::cout << "failed to allocate command buffers!\n";
    }
}

void API::createCommandPool(int graphicsFamilyIndex){
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsFamilyIndex;

    if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        std::cout << "failed to create command pool!\n";
    }
}


void API::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = swapchainFramebuffers[index];

    renderPassBeginInfo.renderArea.extent = swapchainExtent;
    renderPassBeginInfo.renderArea.offset = {0, 0};

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    for (int i = 0; i < Renderer::renderers.size(); i++){
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::renderers[i].pipeline);
        if (Renderer::renderers[i].hasPushConstants){
            vkCmdPushConstants(commandBuffer, Renderer::renderers[i].pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::vec4), &Camera::worldPosition);
            vkCmdPushConstants(commandBuffer, Renderer::renderers[i].pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::vec4), sizeof(glm::mat4), &Camera::rotationMatrix);
        }

        if (Renderer::renderers[i].hasVertexBuffer)
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &Renderer::renderers[i].vertexBuffer, &Renderer::renderers[i].offset);
        
        if (Renderer::renderers[i].hasDescriptorSets){
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::renderers[i].pipelineLayout, 0, 1, &Renderer::renderers[i].descriptorSet, 0, nullptr);
        }
            
        vkCmdDraw(commandBuffer, Renderer::renderers[i].numVertices, 1, 0, 0);
    }
    
    vkCmdEndRenderPass(commandBuffer);
    vkEndCommandBuffer(commandBuffer);

}

