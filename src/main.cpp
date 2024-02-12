#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include "model.h"
#include "shader_m.h"
#include "camera.h"
#include "GUI.h"
#include "Skybox.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float rotationMat[3] = {0.0f,0.0f,0.0f};
//float rotationAngleY = 0.0f;
//float rotationAngleZ = 0.0f;
bool cameraDisabled = false;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool isphongblinn = true;
bool Usequaternions = false;
bool useXAxis = false;
bool useYAxis = false;
bool useZAxis = false;
float angle = 0.0f;
float AxisUse[3] = {0.0f,0.0f,0.0f};

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    GLfloat backGroundColor[3] = {0.3f,0.2f,0.3f};
    // configure global opengl state
    // -----------------------------

    // build and compile shaders
    // -------------------------
    // Shader ourShader(FileSystem::getPath("Resources/Shaders/1.model_loading.vs").c_str(), FileSystem::getPath("Resources/Shaders/1.model_loading.fs").c_str());
    // Model ourModel(FileSystem::getPath("Resources/Plane/plane1.obj"));
    glm::vec3 lightDir(-0.2f, 1.0f, -0.3f);

	// All the faces of the cubemap (make sure they are in this exact order)
	std::string facesCubemap[6] =
	{
		"Resources/Cubemap/right.jpg",
		"Resources/Cubemap/left.jpg",
		"Resources/Cubemap/top.jpg",
		"Resources/Cubemap/bottom.jpg",
		"Resources/Cubemap/front.jpg",
		"Resources/Cubemap/back.jpg"
	};

    Skybox skybox(facesCubemap);

    while (!glfwWindowShouldClose(window))
    {
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glClearColor(backGroundColor[0],backGroundColor[1],backGroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.DrawSkyBox(view,projection);
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

         // input
        // -----
        processInput(window);
        if (cameraDisabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        guiManager.BeginFrame();
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Background Color", backGroundColor);
        ImGui::Checkbox("Use Quaternion", &Usequaternions);
        if(Usequaternions)
        {
            ImGui::Checkbox("X",&useXAxis);
            ImGui::Checkbox("Y",&useYAxis);
            ImGui::Checkbox("Z",&useZAxis);
            ImGui::SliderFloat("Angle", &angle,0.0f,360.0f);

        }
        ImGui::End();
        if(useXAxis)
        {
            AxisUse[0] = 1.0f;
        }
        if(useYAxis){
            AxisUse[1] =  1.0f;
        }
        if(useZAxis)
        {
            AxisUse[2] = 1.0f;
        }
        // render
        // ------

        // don't forget to enable shader before setting uniforms
        // ourShader.use();
        //  // translate it down so it's at the center of the scene
        // //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        // 	// it's a bit too big for our scene, so scale it down
        // glm::quat rotation = glm::quat(1.0f,0.0f,0.0f,0.0f);
        // // view/projection transformations
        // ourShader.setMat4("projection", projection);
        // ourShader.setMat4("view", view);
        // ourShader.setVec3("camPos",camera.Position);
        // // render the loaded model
        // glm::mat4 model = glm::mat4(1.0f);
        // if(!Usequaternions)
        // {
        //     model = glm::rotate(model, rotationMat[2], glm::vec3(0.0f, 0.0f, 1.0f));
        //     model = glm::rotate(model, rotationMat[1], glm::vec3(0.0f, 1.0f, 0.0f));
        //     model = glm::rotate(model, rotationMat[0], glm::vec3(1.0f, 0.0f, 0.0f));
        // }
        // else{
        //         glm::quat rotationQuat = glm::angleAxis(glm::radians(angle), glm::normalize(glm::vec3(AxisUse[0],AxisUse[1],AxisUse[2])));
        //         rotation *=rotationQuat;
        //         rotationMat[0]=0.0f;
        //         rotationMat[1]=0.0f;
        //         rotationMat[2]=0.0f;
        //         AxisUse[0] = 0.0f;
        //         AxisUse[1] = 0.0f;
        //         AxisUse[2] = 0.0f;
        // }
        // model *= glm::mat4_cast(rotation);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        // ourShader.setMat4("model", model);
        // ourShader.setVec3("lightDir", lightDir);
        // ourShader.setBool("isPhongBlinn",isphongblinn);
        // ourModel.Draw(ourShader);
        // isphongblinn = false;

        guiManager.EndFrame();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
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

    // Adjust the rotation angle based on user input (e.g., arrow keys)
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        rotationMat[1] += 0.005f;
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        rotationMat[0] += 0.005f;


    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        rotationMat[2] += 0.005f;
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
