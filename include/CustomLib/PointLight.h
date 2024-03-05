#include "Light.h"

class PointLight : public Light {
public:
    glm::vec3 Position;
    float Constant;
    float Linear;
    float Quadratic;

    PointLight(){}
    PointLight(glm::vec3& color, float intensity, float ambient, float diffuse, float specular, glm::vec3 position,
    float constant, float linear, float quadratic){
        Color = color;
        Ambient = ambient;
        Diffuse = diffuse;
        Specular = specular;
        Intensity = intensity;
        Position = position;
        Constant = constant;
        Linear = linear;
        Quadratic = quadratic;
    }

    void setupLightUniforms() override {}

    void setupLightUniforms(Shader& shader,std::vector<PointLight>& pointLights) {
    shader.use();
    // Send the number of point lights to the shader
    shader.setInt("NumOfPointLights", pointLights.size());
    shader.setFloat("material.shininess",8.0f);

    for (int i = 0; i < pointLights.size(); ++i) {
        // Construct the uniform name for each property
        std::string baseName = "pointLights[" + std::to_string(i) + "].";

        // Set each property of the point light
        shader.setVec3(baseName + "color", pointLights[i].Color.x,pointLights[i].Color.y,pointLights[i].Color.z);
        shader.setFloat(baseName + "intensity", pointLights[i].Intensity);
        shader.setVec3(baseName + "position", pointLights[i].Position.x,pointLights[i].Position.y,pointLights[i].Position.z);
        shader.setFloat(baseName + "ambient", pointLights[i].Ambient);
        shader.setFloat(baseName + "diffuse", pointLights[i].Diffuse);
        shader.setFloat(baseName + "specular", pointLights[i].Specular);
        shader.setFloat(baseName + "constant", pointLights[i].Constant);
        shader.setFloat(baseName + "linear", pointLights[i].Linear);
        shader.setFloat(baseName + "quadratic", pointLights[i].Quadratic);
    }
    }
    void updateLightParameters_Realtime(Shader& shader, std::vector<PointLight>& pointLights,float newIntensity,float LightPosition[],float LightColor[],float intensities[],float attenuation[], int i)
    {
        shader.use();
        std::string baseName = "pointLights[" + std::to_string(i) + "].";

        pointLights[i].Intensity = newIntensity;
        pointLights[i].Position.x = LightPosition[0];
        pointLights[i].Position.y = LightPosition[1];
        pointLights[i].Position.z = LightPosition[2];
        pointLights[i].Color.x = LightColor[0];
        pointLights[i].Color.y = LightColor[1];
        pointLights[i].Color.z = LightColor[2];
        pointLights[i].Ambient = intensities[0];
        pointLights[i].Diffuse = intensities[1];
        pointLights[i].Specular = intensities[2];
        pointLights[i].Constant = attenuation[0];
        pointLights[i].Linear = attenuation[1];
        pointLights[i].Quadratic = attenuation[2];

        shader.setVec3(baseName + "color", pointLights[i].Color.x,pointLights[i].Color.y,pointLights[i].Color.z);
        shader.setFloat(baseName + "intensity", pointLights[i].Intensity);
        shader.setVec3(baseName + "position", pointLights[i].Position.x,pointLights[i].Position.y,pointLights[i].Position.z);
        shader.setFloat(baseName + "ambient", pointLights[i].Ambient);
        shader.setFloat(baseName + "diffuse", pointLights[i].Diffuse);
        shader.setFloat(baseName + "specular", pointLights[i].Specular);
        shader.setFloat(baseName + "constant", pointLights[i].Constant);
        shader.setFloat(baseName + "linear", pointLights[i].Linear);
        shader.setFloat(baseName + "quadratic", pointLights[i].Quadratic);

    }
};
