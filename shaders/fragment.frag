#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {

    vec2 pointCoord = gl_PointCoord * 2.0 - 1.0;
    float dist = length(pointCoord);

    if (dist > 1.0) {
        discard;
    }

    outColor = vec4(fragColor, 1.0);
}

