#include <vector>
#include <cmath>
#include <glm/glm.hpp>

class ColorGradient {
    public:
        std::vector<glm::vec3> colors;    
        glm::vec3 sample(float time);

    private:
};