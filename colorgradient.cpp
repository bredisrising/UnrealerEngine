#include "colorgradient.hpp"

glm::vec3 ColorGradient::sample(float time) {
    size_t numColors = colors.size();

    glm::vec3 startColor = colors[(int)(time*numColors)%numColors];
    glm::vec3 endColor = colors[(int)(time*numColors+1)%numColors];

    float correctedTime = std::fmod(time * numColors, 1.0f);
    glm::vec3 resultColor = glm::mix(startColor, endColor, correctedTime);

    return resultColor;
}