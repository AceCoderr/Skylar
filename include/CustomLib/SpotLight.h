
class Spotlight : public Light {
public:
    glm::vec3 Position;
    glm::vec3 Direction;
    float innerCutoff;
    float outerCutoff;
    float Constant;
    float Linear;
    float Quadratic;

    Spotlight(const glm::vec3& color, float intensity,float ambient,float diffuse,float specular, const glm::vec3& position,
            const glm::vec3& direction, float cutOff, float outerCutOff,float constant,float linear,float quadratic)
    {
        Color = color;
        Ambient = ambient;
        Diffuse = diffuse;
        Specular = specular;
        Intensity = intensity;
        Position = position;
        Constant = constant;
        Linear = linear;
        Quadratic = quadratic;
        innerCutoff = cutOff;
        outerCutoff = outerCutOff;
        Direction = direction;
    }

    void setupLightUniforms() override {}

    void setupLightUniforms(Shader& shader,Spotlight spotlight) {
    shader.use();
    // Send the number of point lights to the shader
    shader.setFloat("material.shininess",8.0f);
    printf("inten - %f\n",spotlight.Intensity);
    printf("x - %f\n",spotlight.Position.x);

    shader.setFloat("spotlight.intensity",spotlight.Intensity);
    shader.setFloat("spotlight.cutOff",glm::cos(glm::radians(spotlight.innerCutoff)));
    shader.setFloat("spotlight.outerCutOff",glm::cos(glm::radians(spotlight.outerCutoff)));
    shader.setFloat("spotlight.ambient",spotlight.Ambient);
    shader.setFloat("spotlight.diffuse",spotlight.Diffuse);
    shader.setFloat("spotlight.specular",spotlight.Specular);
    shader.setFloat("spotlight.constant",spotlight.Constant);
    shader.setFloat("spotlight.linear",spotlight.Linear);
    shader.setFloat("spotlight.quadratic",spotlight.Quadratic);
    shader.setVec3("spotlight.color", spotlight.Color.x,spotlight.Color.y,spotlight.Color.z);
    shader.setVec3("spotlight.position", spotlight.Position.x,spotlight.Position.y,spotlight.Position.z);
    shader.setVec3("spotlight.direction", spotlight.Direction.x,spotlight.Direction.y,spotlight.Direction.z);
    }
};
