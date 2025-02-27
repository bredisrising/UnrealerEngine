#include "engine.hpp"


Engine::Engine(GLFWwindow* window): window(window) {
    api = new API(window);
    api->initializeVulkan();
    circleRenderer = Renderer::createCircleRenderer(api->logicalDevice, api->physicalDevice, api->graphicsPipelineCreateInfo);
    ecsmanager = new ECSManager(*this, 4000);

}