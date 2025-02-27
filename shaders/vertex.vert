#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in float rotation;
layout(location = 2) in float scale;

layout(location = 0) out vec3 fragColor;

float ratio = 1000.0/1000.0;

void main() {
    gl_Position = vec4(inPosition[0] * ratio, inPosition[1], 1.0, 1.0);
    gl_PointSize = scale;

    fragColor = vec3(1.0, 1.0, 1.0);

}