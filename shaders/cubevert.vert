#version 450

layout(push_constant) uniform PushConstants {
    vec4 camPos;
    mat4 rotationMatrix;
} pushconstants;

layout(std430, binding = 0) readonly buffer Cube {
    vec3 position;
    float dim;
} CubeInstance;

// layout (location=0) in vec3 inPosition;
// layout (location=1) in float dimension;



layout (location=0) out vec3 vertPos;
// layout (location=1) out vec3 normal;

vec3 cubeVertices[36] = {
    // front quad
    vec3(-1.0, -1.0, -1.0),
    vec3(1.0, 1.0, -1.0),
    vec3(-1.0, 1.0, -1.0),
    vec3(1.0, -1.0, -1.0),
    vec3(1.0, 1.0, -1.0),
    vec3(-1.0, -1.0, -1.0),

    //back quad
    vec3(-1.0, 1.0, 1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, -1.0, 1.0),
    vec3(1.0, -1.0, 1.0),
    vec3(-1.0, -1.0, 1.0),
    vec3(-1.0, 1.0, 1.0),

    // top quad
    vec3(-1.0, -1.0, -1.0),
    vec3(-1.0, -1.0, 1.0),
    vec3(1.0, -1.0, 1.0),
    vec3(-1.0, -1.0, -1.0),
    vec3(1.0, -1.0, 1.0),
    vec3(1.0, -1.0, -1.0),

    // bottom quad
    vec3(-1.0, 1.0, -1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(-1.0, 1.0, 1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(-1.0, 1.0, -1.0),
    vec3(1.0, 1.0, -1.0),

    // left quad
    vec3(-1.0, 1.0, -1.0),
    vec3(-1.0, 1.0, 1.0),
    vec3(-1.0, -1.0, -1.0),
    vec3(-1.0, 1.0, 1.0),
    vec3(-1.0, -1.0, 1.0),
    vec3(-1.0, -1.0, -1.0),

    // right quad
    vec3(1.0, 1.0, -1.0),
    vec3(1.0, -1.0, -1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, -1.0, -1.0),
    vec3(1.0, -1.0, 1.0)
};

mat4 m = mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 1.0/.8,
    0.0, 0.0, 0.0, 0.0
);

void main () {
    // 36 vertices needed for a cube
    int cubeVertex = gl_VertexIndex % 36;
    vec3 worldSpace = cubeVertices[cubeVertex]+CubeInstance.position;
    vec4 perspective = m * (pushconstants.rotationMatrix * (vec4(worldSpace, 1.0) - pushconstants.camPos));

    gl_Position = perspective;
    gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
    vertPos = cubeVertices[cubeVertex];
    // normal = vec3(0.0, 0.0, 0.0);
}