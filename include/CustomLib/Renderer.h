#include "shader.h"
#include "camera.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "filesystem.h"
#include "GUI.h"
#include "model.h"

const unsigned int SCR_WIDTH = 1320;
const unsigned int SCR_HEIGHT = 980;
float rotationMat[3] = {0.0f,1.0f,0.0f};
//bool isphongblinn = false;
glm::vec3 lightPos(0.0f, 3.0f, 3.0f);


float normal_intensity = 0.0f;
float targetPosition[3] = {0.0f,0.0f,0.0f};
bool IK=false;
int NumberOfBones = 2.0f;
bool PolygonMode = false;
float TextureScale = 1.0f;
unsigned int texture;
bool isSkyboxEnabled = false;
GLfloat backGroundColor[3] = {0.3f,0.2f,0.3f};
int MipmapType = 0;
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

// All the faces of the cubemap (make sure they are in this exact order)
// unsigned int TextureFromFile()
// {
//     //string filename = string(path);
//     //filename = directory + '/' + filename;

//     unsigned int textureID;
//     glGenTextures(1, &textureID);

//     int width, height, nrComponents;
//     unsigned char *data = stbi_load(FileSystem::getPath("Resources/Objects/Textures/checkboard.jpg").c_str(), &width, &height, &nrComponents, 0);
//     if (data)
//     {
//         GLenum format;
//         if (nrComponents == 1)
//             format = GL_RED;
//         else if (nrComponents == 3)
//             format = GL_RGB;
//         else if (nrComponents == 4)
//             format = GL_RGBA;

//         glBindTexture(GL_TEXTURE_2D, textureID);
//         glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);

//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//         stbi_image_free(data);
//     }
//     else
//     {
//         std::cout << "Texture failed to load at path: " <<  std::endl;
//         stbi_image_free(data);
//     }

//     return textureID;
// }
class Render{
    public:
        std::vector<Shader> ShaderList;
        std::vector<Model> ModelList;
        glm::mat4 globalTransform;
        std::vector<Model> BoneList;

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
            Cube.transform.position = glm::vec3(5.0f,0.0f,0.0f);
            ModelList.push_back(Cube);
            return ModelFile;
            //Bone root;
        }

        void CreateLights();

        void RenderScene(Camera camera,GUI guiManager)
        {
            //render
            //Draw all 3D models
            if (PolygonMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();

            if(!ModelList.empty())
            {
                for(Model _3DModel : ModelList)
                {
                    glm::mat4 model1 = glm::mat4(1.0f);
                    ShaderList[0].setMat4("projection", projection);
                    ShaderList[0].setMat4("view", view);
                    ShaderList[0].setVec3("viewPos",camera.Position);
                    ShaderList[0].setVec3("lightPos", lightPos);
                    ShaderList[0].setFloat("normal_strength", normal_intensity);
                    ShaderList[0].setFloat("texScale",TextureScale);
                    model1 = glm::translate(model1, glm::vec3(_3DModel.transform.position.x,_3DModel.transform.position.y,_3DModel.transform.position.z));
                    _3DModel.updatePosition(model1);
                    _3DModel.updateTransform(model1);
                    _3DModel.Draw(ShaderList[0]);
                    _3DModel.restoreTransform();
                }
            }
        }
        // void renderObjects(Camera camera)
        // {
        //     glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //     glm::mat4 view = camera.GetViewMatrix();
        //     // Bind texture
        //     // glActiveTexture(GL_TEXTURE0);
        //     // glBindTexture(GL_TEXTURE_2D, texture);
        //     ShaderList[0].use();
        //     glm::mat4 model = glm::mat4(1.0f);
        //     //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        //     model = glm::translate(model, glm::vec3(0.0f,0.0f,0.0f));
        //     // Use shader program
        //     ShaderList[0].setMat4("projection", projection);
        //     ShaderList[0].setMat4("view", view);
        //     ShaderList[0].setVec3("viewPos",camera.Position);
        //     ShaderList[0].setInt("texture_diffuse1",texture);
        //     ShaderList[0].setMat4("model", model);
        //     ShaderList[0].setVec3("lightPos", lightPos);
        //     ShaderList[0].setFloat("normal_strength", normal_intensity);
        //             // Draw plane
        //     glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        //                     // Unbind VAO
        //     //glBindVertexArray(0);
        // }

        void RenderUI(GUI guiManager)
        {
            guiManager.BeginFrame();
            ImGui::Begin("Settings");
            ImGui::ColorEdit3("Background Color", backGroundColor);
            ImGui::SliderFloat("Normal Strength", &normal_intensity,0.0f,1.0f);
            ImGui::DragFloat3("Target Position",targetPosition,0.1);
            ImGui::Checkbox("IK",&IK);
            ImGui::Checkbox("WireFrame Mode",&PolygonMode);
            ImGui::Checkbox("Enable Skybox", &isSkyboxEnabled);
            ImGui::SliderInt("MipmapType", &MipmapType,0,3);
            ImGui::SliderFloat("Texture Scale",&TextureScale,0.0f,50.0f);
            ImGui::SliderInt("Chain Lenght",&NumberOfBones,1,5);
            // if(ImGui::Button("Update"))
            // {
            //     LoadModels(guiManager);
            // }
            ImGui::End();
            MipmapType = 2;
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
