#include <glm/glm.hpp>

class Light{
public:
    glm::vec3 Color;
    float Intensity;
    float Ambient;
    float Diffuse;
    float Specular;

    Light(){}
    ~Light(){}

    virtual void setupLightUniforms() = 0;
};
