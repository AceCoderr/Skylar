#include <vector>
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>

class Skybox
{
public:
    Shader skyboxShaderObj;
    unsigned int skyboxVAO;
    unsigned int cubemapTexture;
    Skybox();
    Skybox(std::string faces[],std::string directory);
    ~Skybox();
    void DrawSkyBox(glm::mat4 viewMatrix,glm::mat4 projectionMatrix);
};
