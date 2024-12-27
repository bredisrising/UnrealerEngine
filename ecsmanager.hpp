#pragma once

#include <glm/glm.hpp>
#include "game.hpp"
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
        virtual void addEntity(uint32_t entityID) = 0;
        virtual void removeEntity(uint32_t entityID) = 0;
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
        
        std::unordered_map<uint32_t, uint32_t> indexToEntity;
        std::unordered_map<uint32_t, uint32_t> entityToIndex;

    private:
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
        GridBasedCollisionSystem(Engine& engine, ComponentArray<Transform2D>& gridColliders);
        void step();
    private:
        
        void resolveCellCollisions(int xCellIndex, int yCellIndex, int otherXCell, int otherYCell);

        struct GridKey {
            int entityIndex;
            int hashKey;
            GridKey(int h, int ppi): hashKey(h), entityIndex(ppi) {};
            GridKey(): hashKey(INT_MAX), entityIndex(0) {};
        };
        
        static bool compareHashKey(GridKey& key1, GridKey& key2);

        std::vector<GridKey> keys;
        std::vector<int> startIndices;
        
        Engine& engine;
        ComponentArray<Transform2D>& gridColliders;
        Transform2D* gridCollidersPointer;

        
        int numCellsX;
        int numCellsY;
        float cellSize;
        
        void updateCellLookup();
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
        std::vector<ISystemArray*> systemPointers;
        std::vector<IComponentArray*> componentPointers;

    
};
