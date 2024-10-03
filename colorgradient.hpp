#include <vector>
#include <glm/vec3.hpp>

class ColorGradient {
    public:
        std::vector<glm::vec3> colors;    
        glm::vec3 sample(double time);

    private:
};