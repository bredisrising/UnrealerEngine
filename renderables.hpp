#pragma once
#include "glm/glm.hpp"


struct Sphere {
    
};

struct Cube {
    glm::vec3 position;
    float dim;
};

struct Vertex {
    glm::vec3 position;

    Vertex(){};
    Vertex(glm::vec3 pos): position(pos) {};
    Vertex(float x, float y, float z): position(x, y, z) {};
};

