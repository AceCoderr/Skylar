#ifndef CAMERA_H
#define CAMERA_H

#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;
float FOV = 45.0f;
const float scale = 7.135f;
const float nearClipping = 0.1f;
const float farClipping = 100.f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    double mouseXpos;
    double mouseYpos;
    bool isMouseButtonCick;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        isMouseButtonCick = false;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    glm::mat4 Camera::calCulateViewMatrixForToeIn(bool isLeftEye,float iod, const float& distanceToConvergencePoint)
    {
        // float eyeOffset = (isLeftEye ? -1 : 1) * iod / 2.0f;
        // //glm::vec3 rightDirection = glm::normalize(glm::cross(Front,Up));
        // glm::vec3 eyePosition = glm::vec3(Position.x  + eyeOffset,Position.y, Position.z);

        // glm::vec3 focalPoint = Position + Front * DistanceFromCam;

        // float toe_in_Angle = atan(eyeOffset / DistanceFromCam);

        // glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), toe_in_Angle, Up);
        // glm::vec3 rotatedFront = glm::vec3(rotation * glm::vec4(Front, 0.0f));

        // return glm::lookAt(eyePosition,eyePosition + rotatedFront,Up);
        float halfIOD = iod / 2.0f;
        float eyeOffset = isLeftEye ? -halfIOD : halfIOD;
        glm::vec3 rightDirection = glm::normalize(glm::cross(Front, Up));

        // Calculate eye position for the left or right eye
        glm::vec3 eyePosition = Position + rightDirection * eyeOffset;

        // Dynamic toe-in angle calculation
        float toeInAngleRadians = std::atan(halfIOD / distanceToConvergencePoint);
        float rotationDirection = isLeftEye ? 1.0f : -1.0f; // Determine rotation direction based on eye

       glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationDirection * toeInAngleRadians, glm::vec3(0.0f,1.0f,0.0f));

        // Rotate the front vector
        glm::vec3 rotatedFront = glm::vec3(rotationMatrix * glm::vec4(Front, 0.0f));

        // Calculate view matrix for the current eye
        return glm::lookAt(eyePosition, eyePosition + rotatedFront, Up);
    }

    glm::mat4  Camera::calculateFrustumMat(bool isLeftEye, const float& IOD, const float& convergenceDistance, const unsigned int width, const unsigned int height) {
        // Calculate eye offset: half of IOD to the left or right
        float eyeOffset = (isLeftEye ? -1 : 1) * IOD / 2.0f;

        float halfFOV = FOV / 2.0f;
        float top = tan(halfFOV) * nearClipping;
        float bottom = -top;
        float right = (GLfloat(width)/GLfloat(height)) * top;
        float left = -right;

        // Apply the eye position offset to create an asymmetric frustum
        left += eyeOffset * nearClipping / convergenceDistance;
        right += eyeOffset * nearClipping / convergenceDistance;

        // Using GLM to create the frustum matrix
        return glm::frustum(left, right, bottom, top, nearClipping, farClipping);
    }
   glm::mat4 Camera::calculateProjMat(float FOV,const unsigned int width, const unsigned int height) {
        return glm::perspective(glm::radians(FOV), GLfloat(width)/GLfloat(height), nearClipping, farClipping);
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif
