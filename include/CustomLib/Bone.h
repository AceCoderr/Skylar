#ifndef BONE_H
#define BONE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
class Bone {
public:
    Bone();
    Bone(const glm::vec3& position, const glm::vec3& rotationAxis, float rotationAngle, float boneLenght);

    void addChild(Bone* child);
    void setParent(Bone* parent);
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotationAxis, float rotationAngle);
    void update();
    glm::mat4 getTransform() const;

private:
    glm::vec3 m_position;
    glm::vec3 m_rotationAxis;
    float m_rotationAngle;
    std::vector<Bone*> m_children;
    Bone* m_parent = nullptr;
    glm::mat4 m_transform;
    float bone_lenght;
};

#endif // BONE_H
