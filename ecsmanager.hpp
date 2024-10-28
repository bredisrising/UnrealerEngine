#pragma once

#include "renderer.hpp"
#include "api.hpp"
#include <map>
#include <vector>
#include <array>
#include "components.hpp"

// forward declaration
class Engine;

class ISystemArray {
    public:
        virtual ~ISystemArray() = default;
};

class IComponentArray {
    public:
        virtual void addEntity(uint32_t entityID);
        virtual void removeEntity(uint32_t entityID);
        virtual ~IComponentArray() = default;
};


template <typename ComponentType> 
class ComponentArray : public IComponentArray {
    public:
        ComponentArray(): numCurrentlyUsed(0), numCurrentlyAllocated(0) {}
        
        ComponentType getEntity();
        ComponentType* components;
        uint32_t numCurrentlyUsed;
        uint32_t numCurrentlyAllocated;
        
        void addEntity(uint32_t entityID) override {
            uint32_t newIndex = numCurrentlyUsed;
            indexToEntity[newIndex] = entityID;
            entityToIndex[entityID] = newIndex;
            numCurrentlyUsed++; 
        }

        void removeEntity(uint32_t entityID) override {
            // swap remove for efficiency
            // instead of shifting all subsequent elements
            // just swap the index with the last one
            // then pop the actual last one
            uint32_t indexOfRemovedEntity = entityToIndex[entityID];
            uint32_t indexOfLastElement = numCurrentlyUsed - 1;
            components[indexOfRemovedEntity] = components[indexOfLastElement];
            
            uint32_t entityOfLastElement = indexToEntity[indexOfLastElement];
            indexToEntity[indexOfRemovedEntity] = entityOfLastElement;
            entityToIndex[entityOfLastElement] = indexOfRemovedEntity;

            indexToEntity.erase(indexOfLastElement);
            entityToIndex.erase(entityID);

            numCurrentlyUsed--;
        }

        std::unordered_map<uint32_t, uint32_t>& getEntityToIndex() {
            return entityToIndex;
        }

    private:
        std::unordered_map<uint32_t, uint32_t> indexToEntity;
        std::unordered_map<uint32_t, uint32_t> entityToIndex;
         
};


// systems
class Rigidbody2DSystem : public ISystemArray {
    public:
        Rigidbody2DSystem(ComponentArray<Rigidbody2D>& rigidbodies, ComponentArray<Transform2D>& transforms);
        void step();
    private:
        ComponentArray<Rigidbody2D>& rigidbodies;
        ComponentArray<Transform2D>& transforms;
};

class Transform2DSystem: public ISystemArray {
    public:
        Transform2DSystem(Engine& engine, ComponentArray<Transform2D>& transforms);

    private:
        VkBuffer buffer;    
        ComponentArray<Transform2D>& transforms;
        Engine& engine;
};


class CircleRendererSystem : public ISystemArray {
    public:
        CircleRendererSystem(Engine& engine, ComponentArray<CircleRenderer>& circleRenderers);
        void remap();
        void step();
    private:
        Engine& engine;
        ComponentArray<CircleRenderer>& circleRenderers;
        
};


class GridBasedCollisionSystem : public ISystemArray {
    public:
    private:
};



class ECSManager {
    public:
        ECSManager(Engine& engine, uint32_t numComponentsToPreallocate);
        Engine& engine;
        void step();
        uint32_t createEntity(std::vector<char> componentIDs);
        uint32_t numCurrentEntities;
        const uint32_t numComponentsToPreallocate;

    private:
        // systems
        Transform2DSystem* transform2DSystem;
        Rigidbody2DSystem* rigidbody2DSystem;
        CircleRendererSystem* circleRendererSystem;


        //components
        ComponentArray<Transform2D>* transform2Ds;
        ComponentArray<Rigidbody2D>* rigidbody2Ds;
        ComponentArray<Material>* materials;
        ComponentArray<CircleCollider>* circleColliders;
        ComponentArray<CircleRenderer>* circleRenderers;

        // vectors
        std::vector<ISystemArray*> systemPointers;
        std::vector<IComponentArray*> componentPointers;
};
