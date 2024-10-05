#pragma once
#include <vector>
#include "object.hpp"
#include <random>
#include <math.h>
#include <algorithm>

struct Particle {
    glm::vec2 position;
    glm::vec2 positionLast;
    glm::vec2 acceleration;
    float density;
    float property;
    glm::vec<2, int8_t, glm::qualifier::defaultp> cell;
    Particle(): position(), positionLast(), acceleration{0.0f, 0.8f} {
    };
};

struct GridKey {
    int particleIndex;
    int hashKey;
    GridKey(int h, int ppi): hashKey(h), particleIndex(ppi) {};
    GridKey(): hashKey(INT_MAX), particleIndex(0) {};
};

class Fluid {
    public:
        Fluid(int num);
        void step();
        int numParticles;
        int maxParticles;
        std::vector<Circle> particleCircles;
        std::vector<Particle> particles;
        float lastPhysicsTime = 0.0f;
    private:
        void resolvePPCollisions(int xCellIndex, int yCellIndex, int otherXCell, int otherYCell, bool doCollide);
        float smoothingKernel(float radius, float dist);
        float calculateDensity(glm::vec2 samplePoint);
        float calculateProperty(glm::vec2 samplePoint);
        float smoothingKernelDerivative(float radius, float dist);
        static bool compareHashKey(GridKey& key1, GridKey& key2);

        void updateSpatialLookup();
        void createParticle();
        void doPhysicsStep();

        glm::vec2 calcPropertyGradient(glm::vec2 samplePoint);
    
        float collisionDamping = .5f;
        float smoothingRadius = .12f;
        const float mass = .1;
        float targetDensity = .275f;
        float pressureMultiplier = 0.05f;

        float radius = 10.0f;
        float normalizedRadius = radius / HEIGHT;
        float normalizedRadius2 = 2 * normalizedRadius;

        float spawnRate = 100.0f;
        float minSpawnRate = 100.0f;


        float lastParticleCreateTime = 0.0f;

        float physicsStepInterval = 1.0f/120.0f;
        float minPhysicsStepInterval = 1.0f/120.0f;

        std::vector<GridKey> keys;
        std::vector<int> startIndices;
        
        float cellSize = normalizedRadius2 * 3;
        int numCells = 2.0f / cellSize;

        // int numCells = 64;
        // float cellSize = 2.0f / numCells;
    
}; 