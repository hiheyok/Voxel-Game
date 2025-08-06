// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Player/PlayerMovement.h"

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "Client/ClientLevel/ClientCache.h"
#include "Client/Inputs/InputManager.h"
#include "Level/Entity/Mobs/Player.h"
#include "Utils/Math/vectorOperations.h"

void PlayerMovement::Update(Player* player, const InputManager& inputs,
                            ClientCache* clientWorld) {
  if (inputs.CheckAction(InputAction::kToggleCollusion)) {
    enable_collusion_ = !enable_collusion_;
  }

  RotatePlayer(player, inputs);
  MovePlayer(player, inputs, clientWorld);
}

float PlayerMovement::VelocityMovementCurve(float current, float max,
                                            float delta) {
  int currentTime = static_cast<int>(-log(max - current) +
                                     log(max));  // TODO(hiheyok): Fix this

  int x = static_cast<int>(delta - log(max) - currentTime);

  if (x <= 0) {
    x = 0;
  }

  return max - expf(-x);
}

void PlayerMovement::RotatePlayer(Player* player, const InputManager& inputs) {
  static constexpr float kCamSens = 0.1f;

  player->properties_.rotation_ +=
      inputs.GetMouseDisplacement() * glm::vec2(1.0f, -1.0f) * kCamSens;

  if (player->properties_.rotation_.y > 89.9999f)
    player->properties_.rotation_.y = 89.9999f;
  if (player->properties_.rotation_.y < -89.9999f)
    player->properties_.rotation_.y = -89.9999f;
}

void PlayerMovement::MoveRelative(Player* player, float strafe, float up,
                                  float forward, float friction) {
  float f = strafe * strafe + up * up + forward * forward;

  if (f >= 1.0e-4f) {
    f = sqrtf(f);

    f = f < 1.0f ? 1.0f : f;

    f = friction / f;
    strafe = strafe * f;
    up = up * f;
    forward = forward * f;

    float zCoord = sin(player->properties_.rotation_.y * kDegToRad);
    float xCoord = cos(player->properties_.rotation_.y * kDegToRad);

    player->properties_.velocity_.x += strafe * xCoord - forward * zCoord;
    player->properties_.velocity_.y += up;
    player->properties_.velocity_.z += forward * xCoord + strafe * zCoord;
  }
}

void PlayerMovement::MovePlayer(Player* player, const InputManager& inputs,
                                ClientCache* clientWorld) {
  glm::vec3 front(cos(player->properties_.rotation_.x * kDegToRad) *
                      cos(player->properties_.rotation_.y * kDegToRad),
                  0,
                  sin(player->properties_.rotation_.x * kDegToRad) *
                      cos(player->properties_.rotation_.y * kDegToRad));

  front = normalize(front);

  glm::vec3 right = normalize(cross(front, {0.0f, 1.0f, 0.0f}));
  right.y = 0;

  float velocity = player->properties_.max_speed_;

  if (inputs.CheckAction(InputAction::kSprint)) {
    velocity *= 8.f;
  }

  float v = VelocityMovementCurve(Magnitude(player->properties_.velocity_),
                                  velocity, inputs.delta_) /
            sqrtf(2);

  player->properties_.velocity_ = {0.0f, 0.0f, 0.0f};

  if (inputs.CheckAction(InputAction::kWalkForwards)) {
    player->properties_.velocity_ += front * v;
  }
  if (inputs.CheckAction(InputAction::kStrafeLeft)) {
    player->properties_.velocity_ += -right * v;
  }
  if (inputs.CheckAction(InputAction::kWalkBackwards)) {
    player->properties_.velocity_ += -front * v;
  }
  if (inputs.CheckAction(InputAction::kStrafeRight)) {
    player->properties_.velocity_ += right * v;
  }

  if (inputs.CheckAction(InputAction::kMoveDown)) {
    player->properties_.velocity_.y += -VelocityMovementCurve(
        Magnitude(player->properties_.velocity_), velocity, inputs.delta_);
  }

  if (inputs.CheckAction(InputAction::kJump) &&
      (clientWorld->collusion_manager_.IsEntityOnGround(player) &&
       enable_collusion_)) {
    player->properties_.velocity_.y += velocity * 4000;
  }

  if (inputs.CheckAction(InputAction::kMoveUp)) {
    player->properties_.velocity_.y += velocity;
  }

  if (enable_collusion_) {
    float gravity = -80;
    gravity = 0;

    player->properties_.velocity_.y += gravity;

    glm::vec3 time =
        clientWorld->collusion_manager_.ComputeCollisionTimes(player);

    if ((time.x != -1.) && (time.x <= inputs.delta_)) {
      player->properties_.velocity_.x = 0;
    }
    if ((time.y != -1.) && (time.y <= inputs.delta_)) {
      player->properties_.velocity_.y = 0;

      player->properties_.velocity_.x =
          player->properties_.velocity_.x * powf(1.f / 250.f, inputs.delta_);
      player->properties_.velocity_.z =
          player->properties_.velocity_.z * powf(1.f / 250.f, inputs.delta_);
    }
    if ((time.z != -1.) && (time.z <= inputs.delta_)) {
      player->properties_.velocity_.z = 0;
    }
  }

  player->properties_.position_ +=
      player->properties_.velocity_ * inputs.delta_ / 2.f;
  player->properties_.velocity_ =
      player->properties_.velocity_ * powf(3.f / 25.f, inputs.delta_);
}
