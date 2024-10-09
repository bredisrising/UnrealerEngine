#pragma once
#include <vector>
#include "object.hpp"
#include "colorgradient.hpp"
#include "helper.hpp"
#include <random>
#include <math.h>
#include <algorithm>
#include <fstream>
#include "input.hpp"

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
        Fluid();
        void step();
        int numParticles;
        int maxParticles;  
   
        void map(Circle* circles);
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
        ColorGradient colorgradient;


        float collisionDamping = .75f;
        float smoothingRadius = .25f;
        const float mass = .1;
        float targetDensity = .275f;
        float pressureMultiplier = 0.05f;

        float radius = 10.0f;
        float normalizedRadius = radius / HEIGHT;
        float normalizedRadius2 = 2 * normalizedRadius;

        float spawnRate = 750.0f;
        float minSpawnRate = 100.0f;

        float lastParticleCreateTime = 0.0f;

        float physicsStepInterval = 1.0f/480.0f;
        float minPhysicsStepInterval = 1.0f/480.0f;

        std::vector<GridKey> keys;
        std::vector<int> startIndices;

        Helper::Image image;
        bool saved;
        std::vector<float> endParticleInfo;
        std::vector<float> setParticleColors;
        
        float cellSize = normalizedRadius2 * 1; // SET TO TIME 1 FOR BEST FPS
        int numCells = 2.0f / cellSize;

        Circle* mappedCircles; 


        // int numCells = 100;
        // float cellSize = 2.0f / numCells;
    
}; 