#include "PlayerPOV.h"
#include "RenderEngine/Camera/camera.h"

PlayerPOV::PlayerPOV() : camera_{ std::make_unique<Camera>() }{

}

PlayerPOV::~PlayerPOV() = default;

void PlayerPOV::SetPosition(glm::vec3 pos) {
    camera_->position_ = pos;
}

void PlayerPOV::SetRotation(glm::vec2 rot) {
    camera_->yaw_ = rot.x;
    camera_->pitch_ = rot.y;

    camera_->updateCameraVectors();
}

void PlayerPOV::SetFOV(float fov) {
    camera_->fov_ = fov;
}

Camera* PlayerPOV::GetCamera() {
    return camera_.get();
}
