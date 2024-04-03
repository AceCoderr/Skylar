#include "mesh.h"

class VertexPicker{

    public:
    VertexPicker(){}

    glm::vec3 MousePosToWorldRay(int mouseX, int mouseY, int screenWidth, int screenHeight, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {

        // Convert from screen space to NDC
        float x = (2.0f * mouseX) / screenWidth - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / screenHeight;
        glm::vec4 rayClip = glm::vec4(x, y, -1.0, 1.0);

        // Convert to eye coordinates
        glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

        // Convert to world coordinates
        glm::vec3 rayWor = glm::vec3(glm::inverse(viewMatrix) * rayEye);
        rayWor = glm::normalize(rayWor);

        return rayWor;
    }

    int FindClosestVertex(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const std::vector<Vertex>& vertices)
    {
        int closestVertexIndex = -1;
        float closestDistance = std::numeric_limits<float>::max();

        for (size_t i = 0; i < vertices.size(); ++i) {
            float distance = DistancePointToRay(vertices[i].Position, rayOrigin, rayDirection);
            if (distance < closestDistance) {
                closestDistance = distance;
                closestVertexIndex = i;
            }
        }

        return closestVertexIndex;
    }

    float DistancePointToRay(glm::vec3 point, glm::vec3 rayOrigin, glm::vec3 rayDirection) {
        glm::vec3 p1 = point;
        glm::vec3 p2 = rayOrigin;
        glm::vec3 p3 = rayOrigin + rayDirection; // A point along the ray

        float dist = glm::length(glm::cross(p1-p2, p1-p3)) / glm::length(p3-p2);
        return dist;
    }

};
