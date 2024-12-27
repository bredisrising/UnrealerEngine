#pragma once

#include <glm/glm.hpp>

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
    Rigidbody2D(): lastPosition(), velocity(), acceleration(0.0, 0.8), drag(0.0) {}
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