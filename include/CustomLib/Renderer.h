#include "shader.h"
#include "camera.h"
#include "model.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "filesystem.h"


const unsigned int SCR_WIDTH = 1320;
const unsigned int SCR_HEIGHT = 980;
float rotationMat[3] = {0.0f,0.0f,0.0f};
bool isphongblinn = false;
glm::vec3 lightDir(-0.2f, 1.0f, -0.3f);
// All the faces of the cubemap (make sure they are in this exact order)

class Render{
    public:
        std::vector<Shader> ShaderList;
        std::vector<Model> ModelList;
        Render(){
        };
        ~Render(){
        };
        void generateObjects();
        void generateShaders(){
            Shader newShader(FileSystem::getPath("Resources/Shaders/1.model_loading.vs").c_str(), FileSystem::getPath("Resources/Shaders/1.model_loading.fs").c_str());
            this->ShaderList.push_back(newShader);
        }

        string LoadModels(){
            string filename = FileSystem::getPath("Resources/Plane/plane.obj").c_str();
            Model ourModel(filename);
            ModelList.push_back(ourModel);
            return filename;
        }
        void CreateLights();

        void RenderScene(Camera camera){
            // render
            //Draw all 3D models
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            for (Model _3DModel : ModelList)
            {
                //don't forget to enable shader before setting uniforms
                ShaderList[0].use();
                glm::mat4 model = glm::mat4(1.0f);
                // translate it down so it's at the center of the scene
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
                // it's a bit too big for our scene, so scale it down
                glm::quat rotation = glm::quat(1.0f,0.0f,0.0f,0.0f);
                // view/projection transformations
                ShaderList[0].setMat4("projection", projection);
                ShaderList[0].setMat4("view", view);
                ShaderList[0].setVec3("camPos",camera.Position);
                // render the loaded model
                if(true)
                {
                    model = glm::rotate(model, rotationMat[2], glm::vec3(0.0f, 0.0f, 1.0f));
                    model = glm::rotate(model, rotationMat[1], glm::vec3(0.0f, 1.0f, 0.0f));
                    model = glm::rotate(model, rotationMat[0], glm::vec3(1.0f, 0.0f, 0.0f));
                }
                else{
                        // glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), glm::normalize(glm::vec3(AxisUse[0],AxisUse[1],AxisUse[2])));
                        // rotation *=rotationQuat;
                        // rotationMat[0]=0.0f;
                        // rotationMat[1]=0.0f;
                        // rotationMat[2]=0.0f;
                        // AxisUse[0] = 0.0f;
                        // AxisUse[1] = 0.0f;
                        // AxisUse[2] = 0.0f;
                }
                model *= glm::mat4_cast(rotation);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f))  ;
                ShaderList[0].setMat4("model", model);
                ShaderList[0].setVec3("lightDir", lightDir);
                ShaderList[0].setBool("isPhongBlinn",isphongblinn);
                _3DModel.Draw(ShaderList[0]);
                isphongblinn = false;
            }

        }

};
