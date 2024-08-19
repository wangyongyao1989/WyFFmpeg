

#include "OpenGLCamera3D.h"

OpenGLCamera3D::OpenGLCamera3D(vec3 position, vec3 up, float yaw, float pitch) :
        Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), Sensitivity(SENSITIVITY),
        Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

OpenGLCamera3D::OpenGLCamera3D(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw,
                   float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
                                  Sensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

mat4 OpenGLCamera3D::GetViewMatrix() {

    return lookAt(Position, Position + Front, Up);
}

void OpenGLCamera3D::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
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

void OpenGLCamera3D::ProcessXYMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= Sensitivity;
    yoffset *= Sensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }
    Zoom = 45.0f;
    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void OpenGLCamera3D::ProcessScroll(float yoffset) {
    Zoom -= (float) yoffset;
//    if (Zoom < 1.0f)
//        Zoom = 1.0f;
//    if (Zoom > 45.0f)
//        Zoom = 45.0f;
    if (Zoom < 25.0f)
        Zoom = 25.0f;
    if (Zoom > 100.0f)
        Zoom = 100.0f;
}

void OpenGLCamera3D::updateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    // normalize the vectors, because their length gets closer to 0 the more you look up
    // or down which results in slower movement.
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
