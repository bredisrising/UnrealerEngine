#include "game.hpp"
#include "fluid.hpp"
#include <iostream>


Fluid::Fluid(int num): numParticles(0) {
    particleCircles.resize(num);
    particles.resize(num);
    keys.resize(num);
    startIndices.resize(numCells * numCells + 1);
    maxParticles = num;

    std::cout << "Normalized Radius * 2 " << normalizedRadius2 << std::endl;
    std::cout << "Num Cells " << numCells << " by " << numCells << std::endl;
    std::cout << "Cell Size " << cellSize << " by " << cellSize << std::endl;

    // for (int particleIndex = 0; particleIndex < maxParticles; particleIndex++){
    //     // particleCircles[particleIndex].position[0] = -BOUNDX;
    //     // particleCircles[particleIndex].position[1] = -BOUNDY;

    //     particleCircles[particleIndex].position[0] = Random::getRandom() * BOUNDX;
    //     particleCircles[particleIndex].position[1] = Random::getRandom();    
    //     particleCircles[particleIndex].color[0] = Random::getRandom() * .5 + .5;
    //     particleCircles[particleIndex].color[1] = Random::getRandom() * .5 + .5;
    //     particleCircles[particleIndex].color[2] = Random::getRandom() * .5 + .5;

    //     particleCircles[particleIndex].radius = radius;
    //     particles[particleIndex].position = particleCircles[particleIndex].position;

    //     numParticles++;
    // }
}

bool Fluid::compareHashKey(GridKey& key1, GridKey& key2) {
    return key1.hashKey < key2.hashKey;
}

void Fluid::updateSpatialLookup() {
    for (int i = 0; i < numParticles; i++) {
        int gridIndexX = particles[i].position[0] / cellSize + (numCells/2);
        int gridIndexY = particles[i].position[1] / cellSize + (numCells/2);

        int cellKey = gridIndexY * numCells + gridIndexX;

        cellKey = glm::clamp(cellKey, 0, numCells*numCells-1);

        //std::cout << particles[i].position[0] / gridSize + (numCells/2) << " " << particles[i].position[1] / gridSize + (numCells/2) << " ";
        keys[i] = GridKey{cellKey, i};
        //startIndices[i] = INT_MAX;
    }

    std::sort(keys.begin(), keys.end(), Fluid::compareHashKey);

    std::fill(startIndices.begin(), startIndices.end(), INT_MAX);

    for (int i = 0; i < numParticles; i++) {
        int key = keys[i].hashKey;
        int keyPrev = i == 0 ? INT_MAX : keys[i - 1].hashKey;
        if (key != keyPrev) {
            startIndices[key] = i;
        }
    }
}

void Fluid::createParticle() {
    int newIndex = numParticles;

    // particleCircles[newIndex].position[0] = Random::getRandom()*.8;
    // particleCircles[newIndex].position[1] = Random::getRandom()*.8;   
    // particles[newIndex].position = particleCircles[newIndex].position;
    // particles[newIndex].positionLast[0] = particleCircles[newIndex].position[0]-Random::getRandom()*.03f;
    // particles[newIndex].positionLast[1] = particleCircles[newIndex].position[1]-Random::getRandom()*.03f;

    particleCircles[newIndex].position[0] = -BOUNDX + .1;
    particleCircles[newIndex].position[1] = -BOUNDY + .1;   
    particles[newIndex].position = particleCircles[newIndex].position;
    particles[newIndex].positionLast[0] = particleCircles[newIndex].position[0]-0.02f;
    particles[newIndex].positionLast[1] = particleCircles[newIndex].position[1]; 

    particleCircles[newIndex].color[0] = .1;
    particleCircles[newIndex].color[1] = .9;
    particleCircles[newIndex].color[2] = .9;

    particleCircles[newIndex].radius = radius;

    //particles[newIndex].positionLast = particles[newIndex].position - glm::vec2{0.5f, 0.0f};

    numParticles += 1;
}

void Fluid::doPhysicsStep(){
    updateSpatialLookup();
    
    for (int i = 0; i < numCells; i++){
        for (int j = 0; j < numCells; j++){
            resolvePPCollisions(i, j, i, j, false);
            resolvePPCollisions(i, j, i+1, j-1, false);
            resolvePPCollisions(i, j, i-1, j+1, false);
            resolvePPCollisions(i, j, i-1, j-1, false);
            resolvePPCollisions(i, j, i+1, j+1, false);
            resolvePPCollisions(i, j, i+1, j, false);
            resolvePPCollisions(i, j, i, j+1, false);
            resolvePPCollisions(i, j, i-1, j, false);
            resolvePPCollisions(i, j, i, j-1, false);
        }
    }

    for (int i = 0; i < numParticles; i++){
        if (abs(particles[i].position[0]) > BOUNDX - normalizedRadius) {
            float overlap = abs(particles[i].position[0]) - (BOUNDX - normalizedRadius);
            float diff = particles[i].position[0] - particles[i].positionLast[0];
            
            particles[i].position[0] -= overlap * glm::sign(particles[i].position[0]);
            particles[i].positionLast[0] = particles[i].position[0] +
                (diff) * collisionDamping;
        }

        if (abs(particles[i].position[1]) > BOUNDY - normalizedRadius) {
            float overlap = abs(particles[i].position[1]) - (BOUNDY - normalizedRadius);
            float diff = particles[i].position[1] - particles[i].positionLast[1];

            particles[i].position[1] -= overlap * glm::sign(particles[i].position[1]);
            particles[i].positionLast[1] = particles[i].position[1] +
                (diff) * collisionDamping;
        }
    }

    for (int i = 0; i < numParticles; i++) {
        glm::vec2 displacement = particles[i].position - particles[i].positionLast;

        particles[i].positionLast = particles[i].position;

        if (glm::length(displacement) > .02f){
            displacement = glm::normalize(displacement) * .02f;
        }

        particles[i].position = particles[i].position + displacement + particles[i].acceleration * (physicsStepInterval * physicsStepInterval);
        //particles[i].acceleration = {0.0f, 1.0f};

        particleCircles[i].position[0] = particles[i].position[0];
        particleCircles[i].position[1] = particles[i].position[1];
        float colorScaler = glm::min(.85f, glm::length(displacement)*240);
        particleCircles[i].color = {.9 * colorScaler, .9 * (1.0f - colorScaler), .9 * (1.0f - colorScaler)};
    }

}

void Fluid::step() {
    float timeElapsedSinceLastPhysicsStep = Time::currentTime() - lastPhysicsTime;
    float numSteps = timeElapsedSinceLastPhysicsStep * (1.0f / physicsStepInterval);

    float numParticlesToCreate = numParticles >= maxParticles ? 0.0f : timeElapsedSinceLastPhysicsStep * spawnRate;
    //numParticlesToCreate = glm::clamp(numParticlesToCreate, 0U, 100U);
    if (numParticlesToCreate < 1.0f && numParticles < maxParticles) {
        if (Time::currentTime() > lastParticleCreateTime + (1.0f/spawnRate)) {
            createParticle();
            lastParticleCreateTime = Time::currentTime();
        }
    }
    for (int particle = 0; particle < (int) numParticlesToCreate; particle++) {
        createParticle();
    }

    if (numSteps < 1.0f && numSteps > 0.0f) {
        if (Time::currentTime() > lastPhysicsTime + physicsStepInterval) {
            doPhysicsStep();
            lastPhysicsTime = Time::currentTime();
        }
    }
    for (int physicStep = 0; physicStep < (int)numSteps; physicStep++){
        doPhysicsStep();
        lastPhysicsTime = Time::currentTime();
    }
}

void Fluid::resolvePPCollisions (int xCellIndex, int yCellIndex, int otherXCell, int otherYCell, bool doCollide) {    
    int cellkey = yCellIndex * numCells + xCellIndex;
    int otherkey = otherYCell * numCells + otherXCell;

    if (xCellIndex < 0 || xCellIndex > numCells-1)return;
    if (yCellIndex < 0 || yCellIndex > numCells-1)return;
    if (otherXCell < 0 || otherXCell > numCells-1)return;
    if (otherYCell < 0 || otherYCell > numCells-1)return;

    int thisStart = startIndices[cellkey];  
    if (thisStart == INT_MAX) return;
    int otherStart = startIndices[otherkey];
    if (otherStart == INT_MAX) return;

    for (int i = thisStart; i < numParticles; i++){
        if (keys.at(i).hashKey != cellkey) break;
        int thispi = keys.at(i).particleIndex;
        for (int j = otherStart; j < numParticles; j++){
            int thatpi = keys.at(j).particleIndex;
            if (thispi == thatpi) continue;
            if (keys.at(j).hashKey != otherkey) break;
            float distance = glm::length(particles[thispi].position - particles[thatpi].position);
            if (distance < normalizedRadius2){
                // std::cout << " overlapping ";
                glm::vec2 thisParticleVelocity = particles[thispi].position - particles[thispi].positionLast;
                glm::vec2 thatParticleVelocity = particles[thatpi].position - particles[thatpi].positionLast;
                float thisSpeed = glm::length(thisParticleVelocity);
                float thatSpeed = glm::length(thatParticleVelocity);


                // overlapping
                float overlap = (distance - normalizedRadius2) / 2.0f;
                glm::vec2 displacement = particles[thispi].position - particles[thatpi].position;
                glm::vec2 collisionNormal = glm::normalize(displacement);

                glm::vec2 collisionTangent;
                collisionTangent[0] = -collisionNormal[1];
                collisionTangent[1] = collisionNormal[0];

                float thisParticleTan = glm::dot(thisParticleVelocity, collisionTangent);
                float thatParticleTan = glm::dot(thatParticleVelocity, collisionTangent);

                float thisParticleNorm = glm::dot(thisParticleVelocity, collisionNormal);
                float thatParticleNorm = glm::dot(thatParticleVelocity, collisionNormal);

                float m1 = thatParticleNorm;  
                float m2 = thisParticleNorm;  

                float overlapStep = -0.5f * (normalizedRadius2 - distance);
                
                particles[thispi].position -= collisionNormal * overlap;
                particles[thatpi].position += collisionNormal * overlap;

                // std::cout << thisParticleNorm << " ";

                // particles[thispi].positionLast = particles[thispi].position - (collisionTangent * thisParticleTan + collisionNormal * thatParticleNorm);
                // particles[thatpi].positionLast = particles[thatpi].position - (collisionTangent * thatParticleTan + collisionNormal * thisParticleNorm);
            }
            // particles[thispi].positionLast -= collisionNormal * smoothingKernel(normaliedRadius2*4, distance)*.00003f;
            // particles[thatpi].positionLast += collisionNormal * smoothingKernel(normaliedRadius2*4, distance)*.00003f;
        }
    }
}





float Fluid::calculateDensity(glm::vec2 samplePoint) {
    float density = 0.0f;

    for (Particle& p : particles){
        float dist = glm::length(p.position - samplePoint);
        float influence = smoothingKernel(smoothingRadius, dist);
        density += mass * influence;
    }

    return density;
}

glm::vec2 Fluid::calcPropertyGradient(glm::vec2 samplePoint) {
    glm::vec2 propertyGradient{};

    for (Particle& p : particles){
        float dist = glm::length(p.position - samplePoint);
        glm::vec2 dir = (p.position - samplePoint) / dist;
        float slope = smoothingKernelDerivative(smoothingRadius, dist);
        
        propertyGradient += -p.property * dir * slope * mass / p.density;
    }
}

float Fluid::calculateProperty(glm::vec2 samplePoint) {
    float property = 0;

    for (Particle& p : particles){
        float dist = glm::length(p.position - samplePoint);
        float influence = smoothingKernel(smoothingRadius, dist);
        property += mass * influence;
    }
}

float Fluid::smoothingKernelDerivative(float radius, float dist) {
    if (dist >= radius)  return 0.0f;
    
    float f = radius * radius - dist * dist;
    float scale = -24 / (M_PI * glm::pow(radius, 8));
    return scale * dist * f * f;
}

float Fluid::smoothingKernel(float radius, float dist) {
    float volume = M_PI * glm::pow(radius, 8) / 4.0f;
    float value = glm::max(0.0f, radius * radius - dist * dist);
    return value * value * value / volume;
}

