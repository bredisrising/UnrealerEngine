#include "engine.hpp"
#include "ecsmanager.hpp"
#include "string.h"

Transform2DSystem::Transform2DSystem(Engine& engine, ComponentArray<Transform2D>& transform2Ds): engine(engine), transforms(transform2Ds) {
    VkDeviceMemory vertexBufferMemory;
    VkDeviceSize maxBufferSize = sizeof(Transform2D) * transforms.numCurrentlyAllocated;

    engine.api->createBuffer(maxBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        engine.circleRenderer->vertexBuffer, vertexBufferMemory
    );
    std::cout << "Buffer Created!" << std::endl;
    vkMapMemory(engine.api->logicalDevice, vertexBufferMemory, 0, maxBufferSize, 0, (void**)&transforms.components);
    // engine.circleRenderer->vertexBuffer = buffer; // this shouldn't being doing a copy since they are both technically pointers right?

}

CircleRendererSystem::CircleRendererSystem(Engine& engine, ComponentArray<CircleRenderer>& circleRenderers): engine(engine), circleRenderers(circleRenderers) {

}

void CircleRendererSystem::step() {
    engine.circleRenderer->numVertices = circleRenderers.numCurrentlyUsed;
}


Rigidbody2DSystem::Rigidbody2DSystem(ComponentArray<Rigidbody2D>& rigidbodies, ComponentArray<Transform2D>& transforms): rigidbodies(rigidbodies), transforms(transforms) {
    
}

void Rigidbody2DSystem::step() {
    for (std::pair<uint32_t, uint32_t> pair : rigidbodies.getEntityToIndex()) {
        // verlet integration (more accurate than eulers)
        Rigidbody2D& rigidbody = rigidbodies.components[pair.second];
        Transform2D& transform = transforms.components[pair.second];

        glm::vec2 displacement = transform.position - rigidbody.lastPosition;
        rigidbody.lastPosition = transform.position;

        transform.position = transform.position + displacement + (rigidbody.acceleration - displacement*rigidbody.drag) * (PHYSICS_STEP * PHYSICS_STEP);
    }
}


ECSManager::ECSManager(Engine& engine, uint32_t numComponentsToPreallocate): engine(engine), numComponentsToPreallocate(numComponentsToPreallocate) {
    transform2Ds = new ComponentArray<Transform2D>();
    transform2Ds->numCurrentlyAllocated = numComponentsToPreallocate;

    rigidbody2Ds = new ComponentArray<Rigidbody2D>();
    circleColliders = new ComponentArray<CircleCollider>();
    circleRenderers = new ComponentArray<CircleRenderer>();
    std::cout << "Instantiated Components" << std::endl;
    
    // allocation
    rigidbody2Ds->components = new Rigidbody2D[numComponentsToPreallocate];
    circleColliders->components = new CircleCollider[numComponentsToPreallocate];
    memset(circleColliders->components, 0, numComponentsToPreallocate);
    // circleRenderers->components = new CircleRenderer[numComponentsToPreallocate];
    // memset(circleRenderers->components, 0, numComponentsToPreallocate);
    std::cout << "Allocated Components" << std::endl;
    

    componentPointers.push_back(transform2Ds);
    componentPointers.push_back(rigidbody2Ds);
    componentPointers.push_back(circleRenderers);
    componentPointers.push_back(circleColliders);

    transform2DSystem = new Transform2DSystem(engine, *transform2Ds);
    circleRendererSystem = new CircleRendererSystem(engine, *circleRenderers);
    rigidbody2DSystem = new Rigidbody2DSystem(*rigidbody2Ds, *transform2Ds);

    systemPointers.push_back(circleRendererSystem);
    systemPointers.push_back(rigidbody2DSystem);


    for (int i = 0; i < 1; i++) {
        createEntity(std::vector<char>{0, 1, 2});
        transform2Ds->components[i].position = glm::vec2{0.0, 0.0}; 
        transform2Ds->components[i].scale = 25.0; 
        
    }
    
    std::cout << "ECSManager Constructed!" << std::endl;
}


uint32_t ECSManager::createEntity(std::vector<char> componentIDs){
    uint32_t entityID = ECSManager::numCurrentEntities;
    ECSManager::numCurrentEntities += 1;

    for (char& componentID : componentIDs) {
        componentPointers[componentID]->addEntity(entityID);
    }

    return entityID;
}


void ECSManager::step() {
    // system steps

    // 
    // rigidbodies 
    rigidbody2DSystem->step();
    
    // last rendering systems
    circleRendererSystem->step();
}