#include "game.hpp"

float Time::deltatime = 0.0f;
std::random_device Random::rd;
std::mt19937 Random::gen(Random::rd());
std::uniform_real_distribution<> Random::dis(-1.0, 1.0);

glm::vec4 Camera::worldPosition = {0.0f, 0.0f, 0.0f, 0.0f};
glm::vec4 Camera::forward = {0.0f, 0.0f, 1.0f, 0.0f};
glm::vec4 Camera::up = {0.0f, 1.0f, 0.0f, 0.0f};
glm::vec4 Camera::right = {1.0f, 0.0f, 0.0f, 0.0f};

float Camera::moveSpeed = 3.0f;

glm::mat4 Camera::rotationMatrix = glm::eulerAngleXYZ(0, 0, 0);
float Camera::E = .8f;

float Time::currentTime() {
    return glfwGetTime();
}

float Random::getRandom() {
    return Random::dis(Random::gen);
}