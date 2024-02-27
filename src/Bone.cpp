#include "Bone.h"

Bone::Bone(){
    printf("in constructor");
}
Bone::Bone(const glm::vec3& position, const glm::vec3& rotationAxis, float rotationAngle, float boneLenght)
    : m_position(position), m_rotationAxis(rotationAxis), m_rotationAngle(rotationAngle), bone_lenght(boneLenght) {}

void Bone::addChild(Bone* child) {
    m_children.push_back(child);
}

void Bone::setParent(Bone* parent) {
    m_parent = parent;
}

void Bone::setPosition(const glm::vec3& position) {
    m_position = position;
}

void Bone::setRotation(const glm::vec3& rotationAxis, float rotationAngle) {
    m_rotationAxis = rotationAxis;
    m_rotationAngle = rotationAngle;
}

void Bone::update() {
    // Apply local transformation
    glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), m_position);
    localTransform = glm::rotate(localTransform, glm::radians(m_rotationAngle), m_rotationAxis);

    // Combine with parent's transformation
    if (m_parent) {
        glm::mat4 parentTransform = m_parent->getTransform();
        m_transform = parentTransform * localTransform;
    } else {
        m_transform = localTransform;
    }

    // Update children
    for (auto child : m_children) {
        child->update();
    }
}

glm::mat4 Bone::getTransform() const {
    return m_transform;
}
