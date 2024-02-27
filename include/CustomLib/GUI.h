#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>

class GUI{
    private:
        float targetPosition[3] = {0.0f,0.0f,0.0f};
    public:
        void gui_init(GLFWwindow* window);
        void BeginFrame();
        void EndFrame();
        // void setNormalIntensity(float _intensity)
        // {
        //     normal_intensity = _intensity;
        // }
        // float getNormalIntensity()
        // {
        //     return normal_intensity;
        // }
        float* getTargetPosition(){ return this->targetPosition; }
        void setTargetPosition(float value[]) {
            this->targetPosition[0] = value[0];
            this->targetPosition[1] = value[1];
            this->targetPosition[2] = value[2];
            }
};
