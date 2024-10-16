#include "game.hpp"
#include "fluid.hpp"
#include <iostream>

void Fluid::map(Circle* circles){
    mappedCircles = circles;
    Helper::loadImage("./akshai.bmp", image);

    endParticleInfo.resize(maxParticles*2);
    // setParticleColors.resize(maxParticles*3);
    std::ifstream inFile("./positions.bean", std::ios::binary);
    inFile.read(reinterpret_cast<char*>(endParticleInfo.data()), maxParticles*2*sizeof(float));
    inFile.close();

    // float xRatio = (float) image.width / numCells;
    // float yRatio = (float) image.height / numCells;

    for (int i = 0; i < maxParticles; i++) {
        glm::vec2 pos{};
        float x = endParticleInfo[i*2]*.5+.5;
        float y = 1.0-(endParticleInfo[i*2+1]*.5+.5);
        // pos[2] = endParticleInfo[i*3+2];

        glm::vec3 color = image.pixels[(int)(y*(image.height-1))*(image.width) + (int)(x*(image.width-1))];

        // endParticleInfo[i*3] = color[0];
        // endParticleInfo[i*3+1] = color[1];
        // endParticleInfo[i*3+2] = color[2];
        mappedCircles[i].color = color;
    }
}


Fluid::Fluid() {
    int num = numCells*numCells*1.2;
    std::cout << "PARTICLE COUNT " << num << std::endl;
    particles.resize(num);
    keys.resize(num);
    startIndices.resize(numCells * numCells + 1);
    maxParticles = num;


    std::cout << "Normalized Radius * 2 " << normalizedRadius2 << std::endl;
    std::cout << "Num Cells " << numCells << " by " << numCells << std::endl;
    std::cout << "Cell Size " << cellSize << " by " << cellSize << std::endl;


    colorgradient.colors.push_back(glm::vec3{1.0f, 0.0f, 0.0f});
    colorgradient.colors.push_back(glm::vec3{0.0f, 1.0f, 0.0f});
    colorgradient.colors.push_back(glm::vec3{0.0f, 0.0f, 1.0f});
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
    // particles[newIndex].positionLast[0] = particleCircles[newIndex].position[0]-Random::getRandom()*.005f;
    // particles[newIndex].positionLast[1] = particleCircles[newIndex].position[1]-Random::getRandom()*.005f;

    int spawnPos = newIndex % 10;

    mappedCircles[newIndex].position[0] = -BOUNDX + normalizedRadius2 + 0.01;
    mappedCircles[newIndex].position[1] = -.8+spawnPos*(normalizedRadius2+.002);   
    particles[newIndex].position = mappedCircles[newIndex].position;
    particles[newIndex].positionLast[0] = mappedCircles[newIndex].position[0]-0.0075;
    particles[newIndex].positionLast[1] = mappedCircles[newIndex].position[1]; 

    // glm::vec3 color;
    // color[0] = endParticleInfo[newIndex*3];
    // color[1] = endParticleInfo[newIndex*3+1];
    // color[2] = endParticleInfo[newIndex*3+2];

    // std::cout << colors[newIndex] << " ";

    // mappedCircles[newIndex].color = colorgradient.sample(std::fmod(Time::currentTime()*.15f, 1.0f));

    // mappedCircles[newIndex].color[0] = .1;
    // mappedCircles[newIndex].color[1] = .9;
    // mappedCircles[newIndex].color[2] = .9;

    // mappedCircles[newIndex]
    mappedCircles[newIndex].radius = radius;

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
    

        glm::vec2 displacement = particles[i].position - particles[i].positionLast;

        particles[i].positionLast = particles[i].position;

        if (glm::length(displacement) > .0025f){
            displacement = glm::normalize(displacement) * .0025f;
        }

        // if (glm::length(particles[i].position - Input::getMousePos()) < .5) {
        //     particles[i].acceleration += Input::getMouseMove() * 5000.0f;
        // }

        particles[i].position = particles[i].position + displacement + (particles[i].acceleration - displacement*60.0f) * (physicsStepInterval * physicsStepInterval);
        particles[i].acceleration = {0.0f, 0.8f};

        mappedCircles[i].position[0] = particles[i].position[0];
        mappedCircles[i].position[1] = particles[i].position[1];
        // float colorScaler = glm::min(.85f, glm::length(displacement)*240);
        // particleCircles[i].color = {.9 * colorScaler, .9 * (1.0f - colorScaler), .9 * (1.0f - colorScaler)};
    }

}

void Fluid::step() {
    // float timeElapsedSinceLastPhysicsStep = Time::currentTime() - lastPhysicsTime;
    float timeElapsedSinceLastParticleCreation = Time::currentTime() - lastParticleCreateTime;

    // // with interval adaption - SHOULDN'T SLOW DOWN BUT FPS WILL CRASH AND SIMULATION WILL CRAP ITSELF
    // physicsStepInterval = glm::max(Time::deltatime, minPhysicsStepInterval); // adaptive physics rate to prevent fps collapse and slowdown?
    // physicsStepInterval = Time::deltatime;
    // float numSteps = timeElapsedSinceLastPhysicsStep * (1.0f / physicsStepInterval);

    int numParticlesToCreate = spawnRate * (1.0f/FPS);

    // float numParticlesToCreate = numParticles >= maxParticles ? 0.0f : timeElapsedSinceLastParticleCreation * spawnRate;
    
    // for (int particle = 0; particle < (int) numParticlesToCreate; particle++) {
    //     if (numParticles < maxParticles){         
    //         lastParticleCreateTime = Time::currentTime();
    //         createParticle();   
    //     }
    // }
    

    // for (int physicStep = 0; physicStep < (int)numSteps; physicStep++){
    //     doPhysicsStep();
    //     lastPhysicsTime += physicsStepInterval;
    // }

    if (numParticles < maxParticles){
        for (int i = 0; i < numParticlesToCreate; i++){
            if (numParticles < maxParticles){
                lastParticleCreateTime = Time::currentTime();
                createParticle();
            }else{
                break;
            }
        }
    }
        
    
    // no interval adaption - SIMULTATION SHOULD SLOW DOWN
    // if (numParticles < maxParticles && timeElapsedSinceLastParticleCreation > (1.0f / spawnRate)) {
    //     lastParticleCreateTime += 1.0f/spawnRate;
    //     createParticle();
    // ) 

    if (!saved && numParticles >= maxParticles && ((Time::currentTime() - lastParticleCreateTime) > 5.0f)) {
        std::vector<float> positions(maxParticles*2);
        for (int i = 0; i < numParticles; i++) {
            positions[i*2] = particles[i].position[0];
            positions[i*2+1] = particles[i].position[1];
            // positions[i*3+2] = particles[i].position[2];
        }
        std::ofstream file("./positions.bean", std::ios::binary);
        file.write(reinterpret_cast<const char*>(positions.data()), maxParticles*2*sizeof(float));
        file.close();
        saved = true;
        std::cout << " Saved ";
    }


    doPhysicsStep();

    // if (timeElapsedSinceLastPhysicsStep > physicsStepInterval) {
    //     lastPhysicsTime += physicsStepInterval;
    //     doPhysicsStep();
    // }
}

void Fluid::resolvePPCollisions (int xCellIndex, int yCellIndex, int otherXCell, int otherYCell, bool doCollide) {    
    if (xCellIndex < 0 || xCellIndex > numCells-1)return;
    if (yCellIndex < 0 || yCellIndex > numCells-1)return;
    if (otherXCell < 0 || otherXCell > numCells-1)return;
    if (otherYCell < 0 || otherYCell > numCells-1)return;

    int cellkey = yCellIndex * numCells + xCellIndex;
    int otherkey = otherYCell * numCells + otherXCell;


    int thisStart = startIndices[cellkey];
    if (thisStart == INT_MAX) return;
    int otherStart = startIndices[otherkey];
    if (otherStart == INT_MAX) return;

    for (int i = thisStart; keys[i].hashKey == cellkey; i++){
        int thispi = keys[i].particleIndex;
        for (int j = otherStart; keys[j].hashKey == otherkey; j++){
            int thatpi = keys[j].particleIndex;
            if (thispi == thatpi) continue;
            float distance = glm::length(particles[thispi].position - particles[thatpi].position);
            if (distance < normalizedRadius2){
                // std::cout << " overlapping ";
                // glm::vec2 thisParticleVelocity = particles[thispi].position - particles[thispi].positionLast;
                // glm::vec2 thatParticleVelocity = particles[thatpi].position - particles[thatpi].positionLast;
                // float thisSpeed = glm::length(thisParticleVelocity);
                // float thatSpeed = glm::length(thatParticleVelocity);

                // overlapping
                float overlap = (distance - normalizedRadius2) / 2.0f;
                glm::vec2 displacement = particles[thispi].position - particles[thatpi].position;
                glm::vec2 collisionNormal = displacement / distance;

                // glm::vec2 collisionTangent;
                // collisionTangent[0] = -collisionNormal[1];
                // collisionTangent[1] = collisionNormal[0];

                // float thisParticleTan = glm::dot(thisParticleVelocity, collisionTangent);
                // float thatParticleTan = glm::dot(thatParticleVelocity, collisionTangent);

                // float thisParticleNorm = glm::dot(thisParticleVelocity, collisionNormal);
                // float thatParticleNorm = glm::dot(thatParticleVelocity, collisionNormal);

                // float overlapStep = -0.5f * (normalizedRadius2 - distance);
                
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

