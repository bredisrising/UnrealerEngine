#version 450

layout(location = 0) in vec3 fragPos;
// layout(location = 1) in vec3 fragNormal;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
    float edgeThickness = .02;


    // else if (1.0-abs(fragPos.y) <= edgeThickness){
    //     outColor = vec4(0.0, 0.0, 0.0, 1.0);
    // }

    if (1.0-abs(fragPos.x) <= edgeThickness && 1.0-abs(fragPos.y) <= edgeThickness){
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else if (1.0-abs(fragPos.x) <= edgeThickness && 1.0-abs(fragPos.z) <= edgeThickness){
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else if (1.0-abs(fragPos.y) <= edgeThickness && 1.0-abs(fragPos.x) <= edgeThickness){
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else if (1.0-abs(fragPos.y) <= edgeThickness && 1.0-abs(fragPos.z) <= edgeThickness){
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else if (1.0-abs(fragPos.z) <= edgeThickness && 1.0-abs(fragPos.x) <= edgeThickness){
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else if (1.0-abs(fragPos.z) <= edgeThickness && 1.0-abs(fragPos.y) <= edgeThickness){
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }



    // outColor = vec4(1.0, 1.0, 1.0, 1.0);
}