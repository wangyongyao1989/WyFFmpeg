//
// Created by MMM on 2024/8/5.
//

#ifndef ANDROIDLEARNOPENGL_CAMERA3D_H
#define ANDROIDLEARNOPENGL_CAMERA3D_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

using namespace glm;

class OpenGLCamera3D {

private:
    // camera Attributes
    vec3 Up;
    vec3 Right;
    vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float Sensitivity;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

public:
    float Zoom;
    vec3 Position;
    vec3 Front;

    // constructor with vectors
    OpenGLCamera3D(
            vec3 position = vec3(0.0f, 0.0f, 10.0f),
            vec3 up = vec3(0.0f, 1.0f, 0.0f),
            float yaw = YAW,
            float pitch = PITCH);

    // constructor with scalar values
    OpenGLCamera3D(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw,
             float pitch);

    mat4 GetViewMatrix();

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    void ProcessXYMovement(float xoffset, float yoffset, bool constrainPitch = true);

    void ProcessScroll(float yoffset);


};

#endif //ANDROIDLEARNOPENGL_CAMERA3D_H
