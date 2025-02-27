#pragma once

#include <glm/glm.hpp>
#include "game.hpp"
#include "renderer.hpp"
#include "api.hpp"
#include "systems.hpp"
#include <map>
#include <vector>
#include <array>
#include "components.hpp"

// forward declaration
class Engine;

class ISystem {
    public:
        virtual ~ISystem() = default;
};


class ECSManager {
    public:
        ECSManager(Engine& engine, uint32_t numComponentsToPreallocate);
        Engine& engine;
        void step();
        uint32_t createEntity(std::vector<char> componentIDs);
        uint32_t numCurrentEntities;
        uint32_t numComponentsToPreallocate;
       
        // systems
        Transform2DSystem* transform2DSystem;
        Rigidbody2DSystem* rigidbody2DSystem;
        CircleRendererSystem* circleRendererSystem;
        GridBasedCollisionSystem* gridBasedCollisionSystem;

        //components
        ComponentArray<Transform2D>* transform2Ds;
        ComponentArray<Rigidbody2D>* rigidbody2Ds;
        ComponentArray<Material>* materials;
        ComponentArray<Collider2D>* gridColliders;
        ComponentArray<CircleRenderer>* circleRenderers;

        // vectors
        std::vector<ISystem*> systemPointers;
        std::vector<IComponentArray*> componentPointers;

    
};
