#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform vec3 camPos;

vec4 calBlinnPhongLighting()
{
    float ambientIntensity = 0.3f;
    float diffuseIntensity = 0.9f;
    float specularIntensity = 0.7f;
    vec3 lightColor = vec3(1.0f,1.0f,1.0f);
    vec4 ambient = vec4(lightColor,1.0f) * ambientIntensity;
    float diffuse = max(dot(normalize(fs_in.Normal) ,normalize(lightDir)),0.0f);
    vec4 diffuseColour = vec4(diffuse *lightColor* diffuseIntensity,1.0f);
    vec3 fragToEye = normalize(camPos - fs_in.FragPos);
    vec3 reflectedVertex = normalize(reflect(normalize(fs_in.Normal),lightDir));
    vec4 specularColour = vec4(0, 0, 0, 0);
    float specularFactor = dot(fragToEye, reflectedVertex);
    specularFactor = pow(specularFactor, 32.0f);
    //blinn-phong shading for specular highlights
    specularColour = vec4(specularFactor *  lightColor * specularIntensity, 1.0f);
    return (ambient + diffuseColour + specularColour);
}

void main()
{
    vec4 finalColor = vec4(1.0f);
    finalColor = calBlinnPhongLighting();
    FragColor = finalColor * texture(texture_diffuse1, fs_in.TexCoords);//vec3(0.196078f,0.6f,0.8f);
}
