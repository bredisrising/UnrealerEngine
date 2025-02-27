#pragma once

#include "glm/glm.hpp"
#include "game.hpp"
#include "renderer.hpp"
#include "components.hpp"
#include "api.hpp"

// forward declaration to avoid circular dependency
// this is probably just bad in general but idk man im still learning
class Engine;


class ISystem {
    public:
        virtual ~ISystem() = default;
};


// systems
class Rigidbody2DSystem : public ISystem {
    public:
        Rigidbody2DSystem(ComponentArray<Rigidbody2D>& rigidbodies, ComponentArray<Transform2D>& transforms);
        void step();
    private:
        ComponentArray<Rigidbody2D>& rigidbodies;
        ComponentArray<Transform2D>& transforms;
};

class Transform2DSystem: public ISystem {
    public:
        Transform2DSystem(Engine& engine, ComponentArray<Transform2D>& transforms);

    private:
        VkBuffer buffer;    
        ComponentArray<Transform2D>& transforms;
        Engine& engine;
};

class CircleRendererSystem : public ISystem {
    public:
        CircleRendererSystem(Engine& engine, ComponentArray<CircleRenderer>& circleRenderers);
        void remap();
        void step();
    private:
        Engine& engine;
        ComponentArray<CircleRenderer>& circleRenderers;
};


class GridBasedCollisionSystem : public ISystem {
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
