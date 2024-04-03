#include "shader.h"
#include "camera.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "filesystem.h"
#include "GUI.h"
#include "model.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirLight.h"
#include "VertexPicker.h"

//ViewPort settings
const unsigned int SCR_WIDTH = 1320;
const unsigned int SCR_HEIGHT = 980;

//rotation
float rotationMat[3] = {0.0f,1.0f,0.0f};

//Light
///PointLigh
int NumOfPointLights = 0;
glm::vec3 lightPos(0.0f, 3.0f, 3.0f);
float  intensity = 1.0f;
float LightPosition[3] = {0.0f,0.0f,0.0f};
bool isLightAddPressed = false;
float LightColor[3] = {1.0f,1.0f,1.0f};
float AmbientDiffuseSpecular[3] = {1.0f,1.0f,1.0f};
float attenuationFactors[3] = {1.0f,1.0f,1.0f};
///spotlight
bool isSpotlight =false;
bool isSpotlightOn = false;
float spotLightPosition[3] = {0.0f,3.0f,0.0f};
float spotLightColor[3] = {1.0f,1.0f,1.0f};
float spotLightDirection[3] = {0.0f,-1.0f,0.0f};
float spotlightAmbientDiffuseSpecular[3] = {1.0f,1.0f,1.0f};
float spotlightattenuationFactors[3] = {0.07f,0.017f,0.04f};
float spotlightCuttoffs[2] = {7.0f,10.0f};
bool isTorch = false;

//DirLight
bool isDirLight = false;
float lightDir[3] = {1.0f,1.0f,1.0f};
float DirlightAmbientDiffuseSpecular[3] = {1.0f,1.0f,1.0f};
float SunColor[3] = {1.0f,1.0f,1.0f};
float sunIntensity = 1.0f;

//Anaglyph
bool Anaglyph = false;
float IOD = 0.065f;
float convergencePoint = 5.0f;
bool isToein = false;
bool isFrustum = false;

//Animate
float startPos[3] = {0.0f,0.0f,0.0f};
float endPos[3] = {0.0f,0.0f,0.0f};
bool animate = false;

//IK
float targetPosition[3] = {0.0f,0.0f,0.0f};
bool IK=false;
int NumberOfBones = 2.0f;

//skybox and background
bool isSkyboxEnabled = false;
GLfloat backGroundColor[3] = {0.3f,0.2f,0.3f};

//object data
float vertices[] = {
            // Positions        // Texture Coordinates
            -10.0f,  0.0f,  10.0f, 0.0f, 0.0f, // Top-left
            10.0f,  0.0f,  10.f, 1.0f, 0.0f, // Top-right
            -10.0f,  0.0f, -10.0f, 0.0f, 1.0f, // Bottom-left
            10.0f,  0.0f, -10.0f, 1.0f, 1.0f  // Bottom-right
            };
            // Indices for drawing the plane
unsigned int indices[] = {
    0, 1, 2, // First triangle
    1, 3, 2  // Second triangle
};
//texture mapping and mip maps
unsigned int texture;
float TextureScale = 1.0f;
int MipmapType = 0;

//normal mapping
bool isNormalMap = false;
float normal_intensity = 0.0f;

//Wireframe view
bool PolygonMode = false;




class Render{
    public:
        std::vector<Shader> ShaderList;
        std::vector<Model> ModelList;
        glm::mat4 globalTransform;
        std::vector<Model> BoneList;
        std::vector<PointLight> pointlights;
        std::vector<Spotlight> spotlights;
        DirectionLight DirecLight;

        Render(){
            globalTransform = glm::mat4(1.0f);
        };
        ~Render(){
        };
        void generateObjects(Camera camera){

             // Create vertex buffer object (VBO) and element buffer object (EBO)
            unsigned int VBO, EBO;
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            // Create vertex array object (VAO)
            unsigned int VAO;
            glGenVertexArrays(1, &VAO);

            // Bind VAO
            glBindVertexArray(VAO);

            // Bind VBO and copy vertex data
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // Bind EBO and copy index data
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            // Set vertex attribute pointers
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            //texture = TextureFromFile();
        }

        std::vector<Shader> generateShaders(){
            Shader newShader(FileSystem::getPath("Resources/Shaders/1.model_loading.vs").c_str(), FileSystem::getPath("Resources/Shaders/1.model_loading.fs").c_str());
            //Shader ObjectShader(FileSystem::getPath("Resources/Shaders/ObjectShader.vs").c_str(), FileSystem::getPath("Resources/Shaders/ObjectShader.fs").c_str());
            this->ShaderList.push_back(newShader);
            //this->ShaderList.push_back(ObjectShader);
            return ShaderList;
        }

        string LoadModels(GUI guiManager){
            string ModelFile = FileSystem::getPath("Resources/Objects/Brick/BrickObj.obj").c_str();

            Model Cube(ModelFile);
            Cube.transform.position = glm::vec3(0.0f,1.5f,0.0f);
            ModelList.push_back(Cube);

            return ModelFile;
        }

        void CreateLights(){
            //DirectionLight
            DirecLight.Direction.x = lightDir[0];
            DirecLight.Direction.y = lightDir[1];
            DirecLight.Direction.z = lightDir[2];
            DirecLight.Color.x = SunColor[0];
            DirecLight.Color.y = SunColor[1];
            DirecLight.Color.z = SunColor[2];
            DirecLight.Ambient = DirlightAmbientDiffuseSpecular[0];
            DirecLight.Diffuse = DirlightAmbientDiffuseSpecular[1];
            DirecLight.Specular = DirlightAmbientDiffuseSpecular[2];
            DirecLight.Intensity = sunIntensity;

            //creating point lights
            pointlights.clear();
            if(NumOfPointLights > 0)
            {
                PointLight pointLight(glm::vec3(1.0f,1.0f,1.0f),1.0f,1.0f,1.0f,1.0f,glm::vec3(0.0f,3.0f,0.0f),1.0f,1.0f,1.0f);
                for(int i = 0; i < NumOfPointLights; ++i)
                {
                    pointLight.Position.x = pointLight.Position.x + 1.0f;
                    pointLight.Intensity = intensity;
                    pointlights.emplace_back(pointLight);
                }
                pointLight.setupLightUniforms(ShaderList[0],pointlights);
            }
            //SpotLight
            spotlights.clear();
            Spotlight spotlight(glm::vec3(1.0f,1.0f,1.0f),1.0f,1.0f,1.0f,1.0f,glm::vec3(0.0f,3.0f,0.0f),glm::vec3(0.0f,-1.0f,0.0f),7.0f,10.0f,0.07f,0.017f,0.04f);
            spotlights.push_back(spotlight);
            spotlight.setupLightUniforms(ShaderList[0],spotlight);

        }

        void RenderScene(Camera camera,GUI guiManager)
        {

            VertexPicker vertexpick;
            //render
            //Draw all 3D models
            if (PolygonMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            glm::mat4 projection = camera.calculateProjMat(FOV,SCR_WIDTH,SCR_HEIGHT);
            glm::mat4 projection1 = camera.calculateProjMat(FOV,SCR_WIDTH,SCR_HEIGHT);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 view1 = camera.GetViewMatrix();
            if(Anaglyph)
            {
                if(isToein)
                {
                    convergencePoint = glm::distance(camera.Position,ModelList[0].transform.position);
                    view = camera.calCulateViewMatrixForToeIn(1,IOD,convergencePoint);
                    view1 = camera.calCulateViewMatrixForToeIn(0,IOD,convergencePoint);
                    isFrustum = false;
                }
                if(isFrustum)
                {
                    convergencePoint = glm::distance(camera.Position,ModelList[0].transform.position);
                    projection = camera.calculateFrustumMat(1,IOD,convergencePoint,SCR_WIDTH,SCR_HEIGHT);
                    projection1 = camera.calculateFrustumMat(0,IOD,convergencePoint,SCR_WIDTH,SCR_HEIGHT);
                    isToein = false;
                }

            }
            ShaderList[0].use();

            setLightUniforms_realTime(camera);
            std::vector<Mesh> available_meshs;

            if(!ModelList.empty())
            {
                for(Model _3DModel : ModelList)
                {
                    glm::mat4 model1 = glm::mat4(1.0f);
                    ShaderList[0].setMat4("projection", projection);
                    ShaderList[0].setMat4("view", view);
                    ShaderList[0].setVec3("viewPos",camera.Position);
                    ShaderList[0].setFloat("normal_strength", normal_intensity);
                    ShaderList[0].setFloat("texScale",TextureScale);
                    ShaderList[0].setBool("isNormalMap",isNormalMap);
                    //model1 = glm::scale(model1,glm::vec3(0.3f,0.3f,0.3f));
                    //model1 = glm::rotate(model1,glm::radians(rotationMat[1]),glm::vec3(0.0f,1.0f,0.0f));
                    //model1 = glm::translate(model1, glm::vec3(_3DModel.transform.position.x,_3DModel.transform.position.y,_3DModel.transform.position.z));
                    _3DModel.updatePosition(model1);
                    _3DModel.updateTransform(model1);
                    if(Anaglyph)
                    {
                        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
                        glClear(GL_DEPTH_BUFFER_BIT);
                    }
                    _3DModel.Draw(ShaderList[0]);
                    _3DModel.restoreTransform();
                    if(Anaglyph)
                    {
                        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
                        glClear(GL_DEPTH_BUFFER_BIT);
                        ShaderList[0].setMat4("projection", projection1);
                        ShaderList[0].setMat4("view", view1);
                        _3DModel.Draw(ShaderList[0]);
                        _3DModel.restoreTransform();
                        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                        glClear(GL_DEPTH_BUFFER_BIT);
                    }
                    else{
                        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                        glClear(GL_DEPTH_BUFFER_BIT);
                    }
                    for(auto& mesh : _3DModel.meshes){
                        available_meshs.push_back(mesh);
                    }

                }
            }
            if(camera.isMouseButtonCick){
                glm::vec3 ray_world = vertexpick.MousePosToWorldRay(camera.mouseXpos,camera.mouseYpos,SCR_WIDTH,SCR_HEIGHT,view,projection);

                printf("total meshes  - %u\n",available_meshs.size());
                std::vector<Vertex> available_vertices;
                for(auto& mesh : available_meshs)
                {
                    available_vertices.insert(available_vertices.end(),mesh.vertices.begin(),mesh.vertices.end());
                }
                printf("available vertices - %u\n",available_vertices.size());
                int location = vertexpick.FindClosestVertex(camera.Position,ray_world,available_vertices);
                printf("index selected = %d\n",location);
                camera.isMouseButtonCick = false;
            }

        }
        void setLightUniforms_realTime(Camera camera)
        {
            //DirectionalLight update
            DirecLight.Direction.x = lightDir[0];
            DirecLight.Direction.y = lightDir[1];
            DirecLight.Direction.z = lightDir[2];
            DirecLight.Color.x = SunColor[0];
            DirecLight.Color.y = SunColor[1];
            DirecLight.Color.z = SunColor[2];
            DirecLight.Ambient = DirlightAmbientDiffuseSpecular[0];
            DirecLight.Diffuse = DirlightAmbientDiffuseSpecular[1];
            DirecLight.Specular = DirlightAmbientDiffuseSpecular[2];
            DirecLight.Intensity = sunIntensity;
            //SpotLight update
            spotlights[0].Color.x = spotLightColor[0];
            spotlights[0].Color.y = spotLightColor[1];
            spotlights[0].Color.z = spotLightColor[2];
            spotlights[0].Position.x = spotLightPosition[0];
            spotlights[0].Position.y = spotLightPosition[1];
            spotlights[0].Position.z = spotLightPosition[2];
            spotlights[0].Direction.x = spotLightDirection[0];
            spotlights[0].Direction.y = spotLightDirection[1];
            spotlights[0].Direction.z = spotLightDirection[2];
            spotlights[0].Ambient = spotlightAmbientDiffuseSpecular[0];
            spotlights[0].Diffuse = spotlightAmbientDiffuseSpecular[1];
            spotlights[0].Specular = spotlightAmbientDiffuseSpecular[2];
            spotlights[0].Constant = spotlightattenuationFactors[0];
            spotlights[0].Linear = spotlightattenuationFactors[1];
            spotlights[0].Quadratic = spotlightattenuationFactors[2];
            if(isTorch)
            {
                spotlights[0].Position = camera.Position;
                spotlights[0].Direction = camera.Front;
            }
            spotlights[0].setupLightUniforms(ShaderList[0],spotlights[0]);
            DirecLight.setupLightUniforms(ShaderList[0],DirecLight);

        }

        void RenderUI(GUI guiManager){
            guiManager.BeginFrame();
            ImGui::Begin("Settings");
            if(ImGui::BeginTabBar("Options")){
                if (ImGui::BeginTabItem("Environment")) {
                    ImGui::ColorEdit3("Background Color", backGroundColor);
                    ImGui::Checkbox("Enable Skybox", &isSkyboxEnabled);
                    ImGui::Checkbox("WireFrame Mode",&PolygonMode);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Texture Mapping")) {
                    ImGui::SliderFloat("Texture Scale",&TextureScale,0.0f,50.0f);
                    ImGui::Checkbox("Activate Normal Map", &isNormalMap);
                    if(isNormalMap)
                        ImGui::SliderFloat("Normal Strength", &normal_intensity,0.0f,1.0f);
                    ImGui::SliderInt("MipmapType", &MipmapType,0,3);
                    // if(ImGui::Button("Update MipMap type"))
                    // {
                    //     LoadModels(guiManager);
                    // }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Light")) {
                    ImGui::SliderInt("Number Of PointLights",&NumOfPointLights,0,5);
                    if(ImGui::Button("Add Point Light"))
                    {
                        isLightAddPressed = true;
                        if(isLightAddPressed)
                        {
                            //++NumOfPointLights;
                            CreateLights();
                            isLightAddPressed = false;
                        }
                    }
                    if(NumOfPointLights > 0)
                    {
                        int i = 0;
                        for(auto& pointLight : pointlights)
                        {
                            ImGui::ColorEdit3("Light color",LightColor);
                            ImGui::SliderFloat("Intensity",&pointLight.Intensity,0.0f,3.0f);
                            ImGui::DragFloat3("Light Position",LightPosition,0.1);
                            ImGui::Text("Ambient\t\tDiffuse\t\tSpecular");
                            ImGui::DragFloat3("",AmbientDiffuseSpecular,0.1f,0.0f,1.0f);
                            ImGui::Text("Constant\t\tLinear\t\tQuadratic");
                            ImGui::DragFloat3("",attenuationFactors,0.1f,0.0f,1.0f);
                            pointLight.updateLightParameters_Realtime(ShaderList[0],pointlights,pointLight.Intensity,LightPosition,LightColor,AmbientDiffuseSpecular,attenuationFactors,i);
                            ++i;
                        }
                    }
                    ImGui::Checkbox("Add SpotLight",&isSpotlight);
                    if(isSpotlight)
                    {
                        for(auto& spotlight : spotlights)
                        {
                            ImGui::Checkbox("is Torch",&isTorch);
                            ImGui::ColorEdit3("Light color",spotLightColor);
                            ImGui::SliderFloat("Intensity",&spotlight.Intensity,1.0f,10.0f);
                            ImGui::DragFloat3("Light Position",spotLightPosition,0.1f);
                            ImGui::DragFloat3("Light Direction",spotLightDirection,0.1f);
                            ImGui::Text("Ambient\t\tDiffuse\t\tSpecular");
                            ImGui::DragFloat3("",spotlightAmbientDiffuseSpecular,0.1f,0.0f,1.0f);
                            ImGui::Text("Constant\t\tLinear\t\tQuadratic");
                            ImGui::DragFloat3("",spotlightattenuationFactors,0.1f,0.0f,1.0f);
                            ImGui::SliderFloat("Inner Cutoff",&spotlight.innerCutoff,1.0f,100.0f);
                            ImGui::SliderFloat("Outer Cutoff",&spotlight.outerCutoff,1.0f,100.0f);
                            //printf("%f",spotlight.innerCutoff);
                        }
                    }
                    ImGui::Checkbox("Directional Light",&isDirLight);
                    if(isDirLight){
                        ImGui::ColorEdit3("Sun color",SunColor);
                        ImGui::SliderFloat("Intensity",&sunIntensity,0.0f,0.5f);
                        ImGui::Text("Ambient\t\tDiffuse\t\tSpecular");
                        ImGui::DragFloat3("",DirlightAmbientDiffuseSpecular,0.1f,0.0f,1.0f);
                        ImGui::DragFloat3("Sunlight Direction",lightDir,0.1);
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("IK")) {
                    ImGui::Checkbox("IK",&IK);
                    if(IK){
                        ImGui::DragFloat3("Target Position",targetPosition,0.1);
                        ImGui::SliderInt("Chain Lenght",&NumberOfBones,1,5);
                        ImGui::Text("Animate from Start to endPoint");
                        ImGui::DragFloat3("start Position",startPos,0.1);
                        ImGui::DragFloat3("End position",endPos,0.1);
                        if(ImGui::Button("Animate"))
                        {
                            animate = true;
                        }
                        if(ImGui::Button("stop"))
                        {
                            animate = false;
                            startPos[0] = 0.0f;
                            startPos[1] = 0.0f;
                            startPos[2] = 0.0f;
                            endPos[0] = 0.0f;
                            endPos[1] = 0.0f;
                            endPos[2] = 0.0f;
                        }
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Anaglyph")) {
                    ImGui::Checkbox("Anaglyph",&Anaglyph);
                    if(Anaglyph)
                    {
                        ImGui::Checkbox("Toe-in",&isToein);
                        ImGui::Checkbox("Frustum",&isFrustum);
                        ImGui::SliderFloat("IOD",&IOD,0.0f,0.1f);
                        ImGui::SliderFloat("Convergence Point",&convergencePoint,0.0f,10.0f);
                        ImGui::SliderFloat("FOV",&FOV,0.0f,180.0f);
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
            MipmapType = 2;  //constant for now!!
            switch (MipmapType)
            {
            case 0:
            {
                //printf("%d",MipmapType);
                //no mipmapping
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                printf("no mip-map");
                break;
            }
            case 1:
            {
                printf("%d",MipmapType);
                //Nearest Linear mipmap
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                printf("Nearest - linear\n");
                break;
            }
            case 2:
            {
                //printf("%d",MipmapType);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                //printf("Linear-linear\n");
                break;
            }
            case 3:
            {
                printf("%d",MipmapType);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            }
            default:
                break;
            }
        }
};
