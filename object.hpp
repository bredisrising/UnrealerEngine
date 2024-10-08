#pragma once
#include "glm/glm.hpp"


struct Sphere {
    
};

struct Cube {
    glm::vec3 position;
};

struct Circle {
    glm::vec3 position;
    glm::vec3 color;
    float radius;

    Circle(): position(0.0f, 0.0f, 0.0f), radius(20.0f){};
    Circle(float x, float y, float z): position(x, y, z), radius(20.0f){};
};

struct Vertex {
    glm::vec3 position;

    Vertex(){};
    Vertex(glm::vec3 pos): position(pos) {};
    Vertex(float x, float y, float z): position(x, y, z) {};
};

