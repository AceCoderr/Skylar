class DirectionLight : public Light{
    public:
        glm::vec3 Direction;

    DirectionLight(){}

    DirectionLight(glm::vec3& direction,glm::vec3& color,float intensity,float ambient,float diffuse,float specular){
        Direction = direction;
        Color = color;
        Intensity = intensity;
        Ambient = ambient;
        Diffuse = diffuse;
        Specular = specular;
    }
    void setupLightUniforms() override {}

    void setupLightUniforms(Shader& shader,DirectionLight DirLight) {
        shader.setVec3("dirLight.direction",DirLight.Direction.x,DirLight.Direction.y,DirLight.Direction.z);
        shader.setVec3("dirLight.color",DirLight.Color.x,DirLight.Color.y,DirLight.Color.z);
        shader.setFloat("dirLight.intensity",DirLight.Intensity);
        shader.setFloat("dirLight.ambient",DirLight.Ambient);
        shader.setFloat("dirLight.diffuse", DirLight.Diffuse);
        shader.setFloat("dirLight.specular", DirLight.Specular);

    }

};
