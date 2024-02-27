#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string.h>
#include "camera.h"
#include "Renderer.h"
#include "Skybox.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));
bool cameraDisabled = false;
//window
const unsigned int SCR_WIDTH2 = 1320;
const unsigned int SCR_HEIGHT2 = 980;
//cursor
bool firstMouse = true;
float lastX = SCR_WIDTH2 / 2.0f;
float lastY = SCR_HEIGHT2 / 2.0f;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
//msc
string directory;
std::string facesCubemap[6] =
	{
		"/Resources/Cubemap/right.jpg",
		"/Resources/Cubemap/left.jpg",
		"/Resources/Cubemap/top.jpg",
		"/Resources/Cubemap/bottom.jpg",
		"/Resources/Cubemap/front.jpg",
		"/Resources/Cubemap/back.jpg"
	};

//IK Models
string RootFile = FileSystem::getPath("Resources/Objects/bone/Bone1.obj").c_str();
string ChildFile = FileSystem::getPath("Resources/Objects/bone/Bone1.obj").c_str();
string TargetFile = FileSystem::getPath("Resources/Objects/bone/target.obj").c_str();
string EndEffector = FileSystem::getPath("Resources/Objects/bone/target.obj").c_str();

void RenderSkybox(Camera camera, Skybox skybox){
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            skybox.DrawSkyBox(view,projection);
        }

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH2, SCR_HEIGHT2, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    GUI guiManager;
    guiManager.gui_init(window);

    //Creating Renderer
    Render render;
    //generating Shaders
    std::vector<Shader> ShaderList = render.generateShaders();
    //loading Models
    string path = render.LoadModels(guiManager);

#pragma region ik_models
    //Setting Root Bone
    Model RootModel(RootFile);
    RootModel.transform.position = glm::vec3(0.0f,0.0f,0.0f);
    RootModel.setInitialTransform();

    //setting up Child Bones
    Model ChildModel(ChildFile);
    ChildModel.transform.position = glm::vec3(0.0f,1.2f,0.0f);
    glm::mat4 trans1 = ChildModel.setInitialTransform();
    RootModel.addChildren(&ChildModel,trans1);

    // Model EndEffectorModel(EndEffector);
    Model EndEffectorModel(EndEffector);
    EndEffectorModel.transform.position = glm::vec3(0.0f,1.2f,0.0f);
    glm::mat4 trans = EndEffectorModel.setInitialTransform();
    ChildModel.addChildren(&EndEffectorModel,trans);

    Model TargetModel(TargetFile);
    TargetModel.transform.position = glm::vec3(2.0f,3.0f,2.0f);
    TargetModel.setInitialTransform();
#pragma endregion

    //Getting directory Info
    directory = path.substr(0, path.find("/Resources"));

    //SkyBox initialization and implementing shader
    Skybox skybox(facesCubemap,directory);

    //generating objects
    //render.generateObjects(camera);

    //loop
    while (!glfwWindowShouldClose(window))
    {
        //glClearColor(backGroundColor[0],backGroundColor[1],backGroundColor[2], 1.0f);
        glClearColor(backGroundColor[0],backGroundColor[1],backGroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //mouse and keyboard inputs
        processInput(window);

        //enabling disabling camera/cursor as needed based on UI inputs
        if (cameraDisabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        //Rendering skybox as needed
        if(isSkyboxEnabled)
        {
            RenderSkybox(camera,skybox);
        }

        //rendering UI
        render.RenderUI(guiManager);
        //render.renderObjects(camera);
        if(!IK)
        {
            render.RenderScene(camera,guiManager);
        }

        //setting if wireframe/fill mode for objects
        if (PolygonMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        //Projection and view matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

#pragma region ik_calculations
        if(IK)
        {
            // if(NumberOfBones > 1)
            // {
            //     for(int i = 0;i < NumberOfBones; ++i)
            //     {
            //         Model* currBone = EndEffectorModel.parent;
            //         currBone->clearChildren();
            //         currBone->addChildren(new Model(ChildFile),trans);
            //     }
            // }
            //setting and updating target model position
            TargetModel.transform.position = glm::vec3(targetPosition[0],targetPosition[1],targetPosition[2]);

            //setting current Shader
            ShaderList[0].use();

            //transformation matrix and setting uniforms
            glm::mat4 model1 = glm::mat4(1.0f);
            ShaderList[0].setMat4("projection", projection);
            ShaderList[0].setMat4("view", view);
            ShaderList[0].setVec3("viewPos",camera.Position);
            ShaderList[0].setVec3("lightPos", lightPos);
            ShaderList[0].setFloat("normal_strength", normal_intensity);
            model1 = glm::translate(model1, glm::vec3(TargetModel.transform.position.x,TargetModel.transform.position.y,TargetModel.transform.position.z));
            TargetModel.updatePosition(model1);
            TargetModel.updateTransform(model1);
            TargetModel.Draw(ShaderList[0]);
            TargetModel.restoreTransform();

            //CCD implementation
            for (auto itr = 0; itr < 4 ; itr++)
            {
                Model* currBone = EndEffectorModel.parent;
                while(currBone)
                {
                    RootModel.updatePosition();

                    glm::vec3 e_i = glm::normalize(EndEffectorModel.transform.position - currBone->getPosition());
                    //printf("\nx-%f\ty-%f\tz-%f\n",e_i.x,e_i.y,e_i.z);
                    glm::vec3 t_i = glm::normalize(TargetModel.transform.position - currBone->getPosition());

                    double cosTheta = glm::dot(t_i, e_i);
                    glm::vec3 rotateAxis = glm::cross(e_i, t_i);
                    double theta = glm::acos(cosTheta);
                    double thetaA = glm::degrees(theta); // in angle
                    currBone->setRotation(thetaA, rotateAxis, 0); // theta is radians
                    currBone->updatePosition();
                    currBone = currBone->parent;
                }

            }
            RootModel.updateTransform();
            RootModel.Draw(ShaderList[0]);
            RootModel.restoreTransform();
        }
#pragma endregion
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        guiManager.EndFrame();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window,GLFW_KEY_C) == GLFW_PRESS)
    {
        if(cameraDisabled == true)
        {
            cameraDisabled = false;
        }
        else{
            cameraDisabled = true;
        }
    }
    // if(cameraDisabled == true)
    // {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    // }

    // // Adjust the rotation angle based on user input (e.g., arrow keys)
    // if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
    //     rotationMat[1] += 0.005f;
    // }

    // if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    //     rotationMat[0] += 0.005f;


    // if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    //     rotationMat[2] += 0.005f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if(!cameraDisabled)
    {
         camera.ProcessMouseMovement(xoffset, yoffset);
    }

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
