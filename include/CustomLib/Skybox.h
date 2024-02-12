#include <vector>
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>

class Skybox
{
    private:
    Shader skyboxShaderObj;
    unsigned int skyboxVAO;
    unsigned int cubemapTexture;
public:

    Skybox();
    Skybox(std::string faces[]);
    ~Skybox();
    void DrawSkyBox(glm::mat4 viewMatrix,glm::mat4 projectionMatrix);
};
