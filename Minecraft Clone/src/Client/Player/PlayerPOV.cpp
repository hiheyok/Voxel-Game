// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Player/PlayerPOV.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>

#include "RenderEngine/Camera/camera.h"

using std::make_unique;

PlayerPOV::PlayerPOV() : camera_{make_unique<Camera>()} {}

PlayerPOV::~PlayerPOV() = default;

void PlayerPOV::SetPosition(glm::vec3 pos) { camera_->position_ = pos; }

void PlayerPOV::SetRotation(glm::vec2 rot) {
  camera_->yaw_ = rot.x;
  camera_->pitch_ = rot.y;

  camera_->UpdateCameraVectors();
}

glm::vec3 PlayerPOV::GetPosition() const { return camera_->position_; }

void PlayerPOV::SetFOV(float fov) { camera_->fov_ = fov; }

Camera* PlayerPOV::GetCamera() { return camera_.get(); }
