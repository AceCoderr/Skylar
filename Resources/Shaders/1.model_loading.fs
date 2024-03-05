#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

struct Material{
    float shininess;
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
    sampler2D texture_specular1;
};

struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
    float ambient;
    float diffuse;
    float specular;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float ambient;
    float diffuse;
    float specular;
    float constant;
    float linear;
    float quadratic;
};


struct Spotlight {
    vec3 position;
    vec3 color;
    float intensity;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_NUM_POINT_LIGHTS 10

//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float normal_strength;
uniform int NumOfPointLights;
uniform bool isNormalMap;
uniform PointLight pointLights[MAX_NUM_POINT_LIGHTS];
uniform Spotlight spotlight;
uniform DirLight dirLight;
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,vec3 diffuseColor,vec3 specularColor)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * diffuseColor * light.color * light.intensity;
    vec3 diffuse = light.diffuse * diff * diffuseColor* light.color * light.intensity;
    vec3 specular = light.specular * spec * specularColor* light.color * light.intensity;
    return (ambient + diffuse + specular);
}

vec3 calcPointLightEffect(PointLight light, vec3 normal,vec3 fragposition, vec3 viewDir,vec3 diffuseColor,vec3 specularColor)
{
    if(isNormalMap)
    {
        light.position = fs_in.TBN * light.position;
    }
    vec3 lightDir = normalize(light.position - fragposition);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragposition);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
  			     light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient  * diffuseColor * light.color * light.intensity;
    vec3 diffuse  = light.diffuse  * diff * diffuseColor * light.color * light.intensity;
    vec3 specular = light.specular * spec * specularColor * light.color * light.intensity;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalculateSpotlight(Spotlight light1, vec3 normal, vec3 fragPosition, vec3 viewDir,vec3 diffuseColor,vec3 specularColor)
{
    if(isNormalMap)
    {
        light1.position = fs_in.TBN * light1.position;
    }
    vec3 lightDir = normalize(light1.position - fragPosition);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance1 = length(light1.position - fragPosition);
    float attenuation1 = 1.0 / (light1.constant + light1.linear * distance1 + light1.quadratic * (distance1 * distance1));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light1.direction));
    float epsilon = light1.cutOff - light1.outerCutOff;
    float intensity1 = clamp((theta - light1.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient1 = light1.ambient * diffuseColor * light1.color *light1.intensity;
    vec3 diffuse1 = light1.diffuse * diff * diffuseColor * light1.color *light1.intensity;
    vec3 specular1 = light1.specular * spec * specularColor * light1.color*light1.intensity;
    ambient1 *= attenuation1 * intensity1;
    diffuse1 *= attenuation1 * intensity1;
    specular1 *= attenuation1 * intensity1;
    return (ambient1 + diffuse1 + specular1);
}

void main()
{
    vec3 fragPos = vec3(0.0f);
    vec3 viewDir = vec3(0.0f);
    vec3 normal = vec3(0.0f);

    if(isNormalMap)
    {
        vec3 TangenViewPos  = fs_in.TBN * viewPos;
        fragPos  = fs_in.TBN * fs_in.FragPos;
        // obtain normal from normal map in range [0,1]
        normal = mix(vec3(0.225,0.26,0.24),texture(material.texture_normal1, fs_in.TexCoords).rgb,normal_strength);
        // transform normal vector to range [-1,1]
        normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

        viewDir = normalize(TangenViewPos - fragPos);
    }
    else{
        normal = normalize(fs_in.Normal);
        fragPos = fs_in.FragPos;
        viewDir = normalize(viewPos - fragPos);
    }
    //get color from the texture
    vec3 diffuseColor = texture(material.texture_diffuse1,fs_in.TexCoords).rgb;
    vec3 specularColor = texture(material.texture_specular1,fs_in.TexCoords).rgb;

    vec3 result = CalcDirLight(dirLight, normal, viewDir,diffuseColor,specularColor); // Initialize final color

    // Calculate point light contribution
    for(int i = 0; i < NumOfPointLights; i++) {
        result += calcPointLightEffect(pointLights[i], normal, fragPos, viewDir, diffuseColor,specularColor);
    }

    result += CalculateSpotlight(spotlight,normal, fragPos, viewDir, diffuseColor,specularColor);

    FragColor = vec4(result,1.0f);
}
