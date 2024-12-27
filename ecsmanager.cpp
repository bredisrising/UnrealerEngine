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
    for (int i = 0; i < rigidbodies.numCurrentlyUsed; i++) {
        // verlet integration (more accurate than eulers)
        Rigidbody2D& rigidbody = rigidbodies.components[i];
        Transform2D& transform = transforms.components[i];

        glm::vec2 displacement = transform.position - rigidbody.lastPosition;
        rigidbody.lastPosition = transform.position;
        transform.position = transform.position + displacement + (rigidbody.acceleration - displacement*rigidbody.drag) * (PHYSICS_STEP * PHYSICS_STEP);

        if (abs(transform.position[0]) > BOUNDX - 15.0f/HEIGHT) {
            float overlap = abs(transform.position[0]) - (BOUNDX - 15.0f/HEIGHT);
            float diff = transform.position[0] - rigidbody.lastPosition[0];
            
            transform.position[0] -= overlap * glm::sign(transform.position[0]);
            rigidbody.lastPosition[0] = transform.position[0] +
                (diff) * .5f;
        }

        if (abs(transform.position[1]) > BOUNDY - 15.0f/HEIGHT) {
            float overlap = abs(transform.position[1]) - (BOUNDY - 15.0f/HEIGHT);
            float diff = transform.position[1] - rigidbody.lastPosition[1];

            transform.position[1] -= overlap * glm::sign(transform.position[1]);
            rigidbody.lastPosition[1] = transform.position[1] +
                (diff) * .5f;
        }
    }
}


GridBasedCollisionSystem::GridBasedCollisionSystem(Engine& engine, ComponentArray<Transform2D>& gridColliders): engine(engine), gridColliders(gridColliders), cellSize(15.0f/HEIGHT*2) {
    numCellsX = 2.0f / cellSize;
    numCellsY = 2.0f / cellSize;
    std::cout << "Num Cells " << numCellsX * numCellsY << std::endl;
    startIndices.resize(numCellsX * numCellsY + 1);
    keys.resize(gridColliders.numCurrentlyUsed);


    gridCollidersPointer = gridColliders.components;

}

void GridBasedCollisionSystem::step(){
    updateCellLookup();
    // for (int i = 0; i < numCellsX; i++) {
    //     for (int j = 0; j < numCellsY; j++) {
    //         resolveCellCollisions(i, j, i, j);
    //         resolveCellCollisions(i, j, i+1, j-1);
    //         resolveCellCollisions(i, j, i-1, j+1);
    //         resolveCellCollisions(i, j, i-1, j-1);
    //         resolveCellCollisions(i, j, i+1, j+1);
    //         resolveCellCollisions(i, j, i+1, j);
    //         resolveCellCollisions(i, j, i, j+1);
    //         resolveCellCollisions(i, j, i-1, j);
    //         resolveCellCollisions(i, j, i, j-1);
    //     }
    // }
    
}

bool GridBasedCollisionSystem::compareHashKey(GridKey& key1, GridKey& key2) {
    return key1.hashKey < key2.hashKey;
}

void GridBasedCollisionSystem::updateCellLookup() {
    for (int componentIndex = 0; componentIndex < gridColliders.numCurrentlyUsed; componentIndex++) {
        // Transform2D& transform = engine.ecsmanager->transform2Ds->components[componentIndex];
        
        // int gridIndexX = 0.0f / cellSize + (numCellsX/2);
        // int gridIndexY = 0.0f / cellSize + (numCellsY/2);
        int gridIndexX = gridCollidersPointer[componentIndex].position[0] / cellSize + (numCellsX/2);
        int gridIndexY = gridCollidersPointer[componentIndex].position[1] / cellSize + (numCellsY/2);
        
        int cellKey = gridIndexY * numCellsX + gridIndexX;
        cellKey = glm::clamp(cellKey, 0, numCellsX*numCellsY-1);
        
        keys[componentIndex].hashKey = cellKey;
        keys[componentIndex].entityIndex = componentIndex;
    }

    std::sort(keys.begin(), keys.end(), GridBasedCollisionSystem::compareHashKey);
    std::fill(startIndices.begin(), startIndices.end(), INT_MAX);
    
    for (int i = 0; i < gridColliders.numCurrentlyUsed; i++) {
        int key = keys[i].hashKey;
        int keyPrev = i == 0 ? INT_MAX : keys[i-1].hashKey;
        if (key != keyPrev) {
            startIndices[key] = i;
        }
    }
}


void GridBasedCollisionSystem::resolveCellCollisions (int xCellIndex, int yCellIndex, int otherXCell, int otherYCell) {    
    if (xCellIndex < 0 || xCellIndex > numCellsX-1)return;
    if (yCellIndex < 0 || yCellIndex > numCellsY-1)return;
    if (otherXCell < 0 || otherXCell > numCellsX-1)return;
    if (otherYCell < 0 || otherYCell > numCellsY-1)return;

    int cellkey = yCellIndex * numCellsX + xCellIndex;
    int otherkey = otherYCell * numCellsX + otherXCell;

    int thisStart = startIndices[cellkey];
    if (thisStart == INT_MAX) return;
    int otherStart = startIndices[otherkey];
    if (otherStart == INT_MAX) return;

    for (int i = thisStart; keys[i].hashKey == cellkey && i < keys.size(); i++){
        int thispi = keys[i].entityIndex;
        for (int j = otherStart; keys[j].hashKey == otherkey && i < keys.size(); j++){
            int thatpi = keys[j].entityIndex;
            if (thispi == thatpi) continue;
            float normalizedWidthProbably = gridColliders.components[thatpi].scale + gridColliders.components[thispi].scale;
            float distance = glm::length(gridColliders.components[thatpi].position - gridColliders.components[thispi].position);
            if (distance < 15.0f/HEIGHT*2){
                // overlapping
                float overlap = (distance - 15.0/HEIGHT*2) / 2.0f;
                glm::vec2 displacement = gridColliders.components[thatpi].position - gridColliders.components[thispi].position;
                glm::vec2 collisionNormal = displacement / distance;
               
                gridColliders.components[thispi].position += collisionNormal * overlap;
                gridColliders.components[thatpi].position -= collisionNormal * overlap;
            }
        }
    }
}


ECSManager::ECSManager(Engine& engine, uint32_t _numComponentsToPreallocate): engine(engine), numComponentsToPreallocate(_numComponentsToPreallocate) {
    transform2Ds = new ComponentArray<Transform2D>();
    transform2Ds->numCurrentlyAllocated = numComponentsToPreallocate;

    rigidbody2Ds = new ComponentArray<Rigidbody2D>();
    gridColliders = new ComponentArray<Collider2D>();
    circleRenderers = new ComponentArray<CircleRenderer>(); // empty component arrary 
    std::cout << "Instantiated Components" << std::endl;
   
    // allocation
    rigidbody2Ds->components = new Rigidbody2D[numComponentsToPreallocate];
    gridColliders->components = new Collider2D[numComponentsToPreallocate];
    
    std::cout << "\nAllocated Components: " << numComponentsToPreallocate << " Each" << std::endl;
    
    double componentMemoryUsage = sizeof(Rigidbody2D) * numComponentsToPreallocate + sizeof(Transform2D) * numComponentsToPreallocate + sizeof(Collider2D) * numComponentsToPreallocate;
    std::cout << "Estimated Component Memory Usage: " << componentMemoryUsage / 1000000 << "MB" << std::endl;
    

    componentPointers.push_back(transform2Ds);
    componentPointers.push_back(rigidbody2Ds);
    componentPointers.push_back(circleRenderers);
    componentPointers.push_back(gridColliders);


    transform2DSystem = new Transform2DSystem(engine, *transform2Ds);
    circleRendererSystem = new CircleRendererSystem(engine, *circleRenderers);
    rigidbody2DSystem = new Rigidbody2DSystem(*rigidbody2Ds, *transform2Ds);
    std::cout << this->numComponentsToPreallocate << std::endl;
    gridBasedCollisionSystem = new GridBasedCollisionSystem(engine, *transform2Ds);
    
    systemPointers.push_back(circleRendererSystem);
    systemPointers.push_back(rigidbody2DSystem);
    systemPointers.push_back(gridBasedCollisionSystem);


    for (int i = 0; i < numComponentsToPreallocate; i++) {
        createEntity(std::vector<char>{0, 1, 2, 3});
        transform2Ds->components[i].position = glm::vec2{Random::getRandom()*ASPECT_RATIO, Random::getRandom()}; 
        transform2Ds->components[i].scale = 15.0; 
        rigidbody2Ds->components[i].lastPosition = transform2Ds->components[i].position;
        gridColliders->components[i].type = Collider2D::Type::Circle;
        gridColliders->components[i].circle.radius = 15.0/HEIGHT;
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


    // gridBasedCollisionSystem->step();
    
    // rigidbodies 
    rigidbody2DSystem->step();
    
    // last rendering systems
    circleRendererSystem->step();
}