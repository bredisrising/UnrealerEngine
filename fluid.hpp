#pragma once
#include <vector>
#include "object.hpp"
#include <random>
// #include "input.hpp"
#include <math.h>
#include <algorithm>

struct Particle {
    glm::vec2 position;
    glm::vec2 positionLast;
    glm::vec2 acceleration;
    float density;
    float property;
    glm::vec<2, int8_t, glm::qualifier::defaultp> cell;
    Particle(): position(), positionLast(), acceleration{0.0f, 0.7f} {
    };
};

struct GridKey {
    int particleIndex;
    int hashKey;
    GridKey(uint32_t h, int ppi): hashKey(h), particleIndex(ppi) {};
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
    private:
        void resolvePPCollisions(int xCellIndex, int yCellIndex, int otherXCell, int otherYCell, bool doCollide);
        float smoothingKernel(float radius, float dist);
        float calculateDensity(glm::vec2 samplePoint);
        float calculateProperty(glm::vec2 samplePoint);
        float smoothingKernelDerivative(float radius, float dist);
        static bool compareHashKey(GridKey& key1, GridKey& key2);

        void updateSpatialLookup();

        glm::vec2 calcPropertyGradient(glm::vec2 samplePoint);
    
        float collisionDamping = .75f;
        float smoothingRadius = .12f;
        const float mass = .1;
        float targetDensity = .275f;
        float pressureMultiplier = 0.05f;

        float particleRadius = 17.5f;
        float radius = 17.5f;
        float normalizedRadius = radius / HEIGHT;
        float normaliedRadius2 = 2 * normalizedRadius;

        float spawnRate = 50.0f;
        float lastTime = 0.0f;

        float lastPhysicsTime = 0.0f;
        float physicsRate = 1.0f/60.0f;

        std::vector<GridKey> keys;
        std::vector<int> startIndices;
        int numCells = 10;

        float gridSize = 1.0f / (numCells / 2.0f);
};  