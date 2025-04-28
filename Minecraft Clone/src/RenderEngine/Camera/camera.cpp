#include "RenderEngine/Camera/Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front_(glm::vec3(0.0f, 0.0f, -1.0f)),
      movement_speed_(SPEED),
      mouse_sensitivity_(SENSITIVITY),
      zoom_(ZOOM) {
    position_ = position;
    world_up_ = up;
    yaw_ = yaw;
    pitch_ = pitch;
    UpdateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
    : front_(glm::vec3(0.0f, 0.0f, -1.0f)),
      movement_speed_(SPEED),
      mouse_sensitivity_(SENSITIVITY),
      zoom_(ZOOM) {
    position_ = glm::vec3(posX, posY, posZ);
    world_up_ = glm::vec3(upX, upY, upZ);
    yaw_ = yaw;
    pitch_ = pitch;
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::GetModelMatrix() { return glm::mat4(1.0f); }

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = movement_speed_ * deltaTime;
    if (direction == CAM_FORWARD) position_ += front_ * velocity;
    if (direction == CAM_BACKWARD) position_ -= front_ * velocity;
    if (direction == CAM_LEFT) position_ -= right_ * velocity;
    if (direction == CAM_RIGHT) position_ += right_ * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
    xoffset *= mouse_sensitivity_;
    yoffset *= mouse_sensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    zoom_ -= (float)yoffset;
    if (zoom_ < 1.0f) zoom_ = 1.0f;
    if (zoom_ > 70.0f) zoom_ = 70.0f;
}

void Camera::UpdateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    // also re-calculate the Right and Up vector
    right_ = glm::normalize(glm::cross(
        front_, world_up_));  // normalize the vectors, because their length
                              // gets closer to 0 the more you look up or down
                              // which results in slower movement.
    up_ = glm::normalize(glm::cross(right_, front_));
}