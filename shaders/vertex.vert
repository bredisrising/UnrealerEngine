#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in float radius;

layout(location = 0) out vec3 fragColor;

float ratio = 1920.0/1920.0;

void main() {
    int quadIndex = gl_VertexIndex % 6;

    gl_Position = vec4(inPosition[0] * ratio, inPosition[1], inPosition[2], 1.0);
    gl_PointSize = radius;

    fragColor = inColor;

}