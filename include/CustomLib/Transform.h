#include <glm/glm.hpp>

struct Transform{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 getPosition()
    {
        return glm::vec3(position.x,position.y,position.z);
    }
};
