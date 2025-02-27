#pragma once

#include <glm/glm.hpp>


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



//components
struct Transform2D {
    glm::vec2 position;
    float rotation;
    float scale;
    Transform2D(): position(), rotation(), scale(15.0f) {}
};


// will use uniform buffer for this?
struct Material {
    glm::vec3 color;
};

struct Rigidbody2D {
    glm::vec2 lastPosition;
    glm::vec2 velocity;
    glm::vec2 acceleration;
    float drag;
    Rigidbody2D(): lastPosition(), velocity(), acceleration(0.0, 0.8), drag(120.0f) {}
};



// general collider for tracking
struct Collider2D {
    enum class Type {Circle, Box};
    Type type;
    union { 
        struct {float radius;} circle;
        struct {glm::vec2 topleft; glm::vec2 topright;} box;
    };
};

// type of collision system
struct GridBasedColliderSystemTagComponent { 

};


struct CircleRenderer {

};