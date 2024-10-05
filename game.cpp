#include "game.hpp"

float Time::deltatime = 0.0f;
std::random_device Random::rd;
std::mt19937 Random::gen(Random::rd());
std::uniform_real_distribution<> Random::dis(-1.0, 1.0);

float Time::currentTime() {
    return glfwGetTime();
}

float Random::getRandom() {
    return Random::dis(Random::gen);
}