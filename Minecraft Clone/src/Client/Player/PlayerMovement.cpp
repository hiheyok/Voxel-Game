#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <cmath>
#include "PlayerMovement.h"
#include "../ClientLevel/ClientCache.h"
#include "../IO/IO.h"
#include "../IO/KEY_CODE.h"
#include "../../Level/Entity/Mobs/Player.h"
#include "../../Utils/Math/vectorOperations.h"

void PlayerMovement::Update(Player* player, const UserInputs& inputs, ClientCache* clientWorld) {

    if (inputs.CheckKeyPress(KEY_C)) {
        enable_collusion_ = !enable_collusion_;
    }

    RotatePlayer(player, inputs);
    MovePlayer(player, inputs, clientWorld);
}

float PlayerMovement::VelocityMovementCurve(float current, float max, float delta) {

    int currentTime = static_cast<int>(-log(max - current) + log(max)); //TODO: Fix this

    int x = static_cast<int>(delta - log(max) - currentTime);

    if (x <= 0) {
        x = 0;
    }

    return max - expf(-x);
}

void PlayerMovement::RotatePlayer(Player* player, const UserInputs& Inputs) {
    float CamSensitivity = 0.1f;

    player->properties_.rotation_.x += static_cast<float>(Inputs.mouse_.displacement_.x * CamSensitivity);
    player->properties_.rotation_.y -= static_cast<float>(Inputs.mouse_.displacement_.y * CamSensitivity);

    if (player->properties_.rotation_.y > 89.9999f)
        player->properties_.rotation_.y = 89.9999f;
    if (player->properties_.rotation_.y < -89.9999f)
        player->properties_.rotation_.y = -89.9999f;
}

void PlayerMovement::MoveRelative(Player* player, float strafe, float up, float forward,  float friction) {
    float f = strafe * strafe + up * up + forward * forward;

    if (f >= 1.0e-4f) {
        f = sqrtf(f);

        f = f < 1.0f ? 1.0f : f;
        
        f = friction / f;
        strafe = strafe * f;
        up = up * f;
        forward = forward * f;

        float zCoord = sin(player->properties_.rotation_.y * 0.017453292f);
        float xCoord = cos(player->properties_.rotation_.y * 0.017453292f);

        player->properties_.velocity_.x += strafe * xCoord - forward * zCoord;
        player->properties_.velocity_.y += up;
        player->properties_.velocity_.z += forward * xCoord + strafe * zCoord;
    }
}

void PlayerMovement::MovePlayer(Player* player, const UserInputs& inputs, ClientCache* clientWorld) {

    glm::vec3 front(
        cos(player->properties_.rotation_.x * 0.0174533) * cos(player->properties_.rotation_.y * 0.0174533),
        0,
        sin(player->properties_.rotation_.x * 0.0174533) * cos(player->properties_.rotation_.y * 0.0174533)
    );

    front = normalize(front);

    glm::vec3 right = normalize(cross(front, glm::vec3(0.0, 1.0, 0.0)));
    right.y = 0;

    float velocity = player->properties_.max_speed_;

    if (inputs.CheckKey(KEY_LEFT_CONTROL)) {
        velocity *= 8.f;
    }

    float v = VelocityMovementCurve(Magnitude(player->properties_.velocity_), velocity, inputs.delta_) / sqrtf(2);

    player->properties_.velocity_ = glm::vec3(0.f, 0.f, 0.f);

    if (inputs.CheckKey(KEY_W)) {
        player->properties_.velocity_ += front * v;
    }
    if (inputs.CheckKey(KEY_A)) {
        player->properties_.velocity_ += -right * v;
    }
    if (inputs.CheckKey(KEY_S)) {
        player->properties_.velocity_ += -front * v;
    }
    if (inputs.CheckKey(KEY_D)) {
        player->properties_.velocity_ += right * v;
    }

    if (inputs.CheckKey(KEY_LEFT_SHIFT)) {
        player->properties_.velocity_.y += -VelocityMovementCurve(Magnitude(player->properties_.velocity_), velocity, inputs.delta_);
    }


    if (inputs.CheckKey(KEY_SPACE) && (clientWorld->collusion_manager_.isEntityOnGround(player) && enable_collusion_)) {
        player->properties_.velocity_.y += velocity * 4000;
    }

    if (inputs.CheckKey(KEY_SPACE)) {
        player->properties_.velocity_.y += velocity;
    }


    if (enable_collusion_) {
        float gravity = -80;
        gravity = 0;

        player->properties_.velocity_.y += gravity;

        glm::vec3 time = clientWorld->collusion_manager_.GetTimeTillCollusion(player);

        if ((time.x != -1.) && (time.x <= inputs.delta_)) {
            player->properties_.velocity_.x = 0;
        }
        if ((time.y != -1.) && (time.y <= inputs.delta_)) {
            player->properties_.velocity_.y = 0;

            player->properties_.velocity_.x = player->properties_.velocity_.x * powf(1.f / 250.f, inputs.delta_);
            player->properties_.velocity_.z = player->properties_.velocity_.z * powf(1.f / 250.f, inputs.delta_);
        }
        if ((time.z != -1.) && (time.z <= inputs.delta_)) {
            player->properties_.velocity_.z = 0;
        }
    }


    player->properties_.position_ += player->properties_.velocity_ * inputs.delta_ / 2.f;
    player->properties_.velocity_ = player->properties_.velocity_ * powf(3.f / 25.f, inputs.delta_);
    /*float jumpMovementFactor = 0.02F;
    float f = 0.91;

    float blockSlipperiness = 0.6;

    if (server->checkPlayerOnGround() && m_EnableCollusion) {
        f = blockSlipperiness * 0.91;
    }

    float f2 = 0.16277136F / (f * f * f);
    float friction = 0.f;

    if (server->checkPlayerOnGround() && m_EnableCollusion) {
        friction = f2 * 0.08;
    }
    else {
        friction = jumpMovementFactor;
    }

    float baseAcceleration = 0.1f;
    if (Inputs.CheckKey(KEY_LEFT_SHIFT)) {
        baseAcceleration = 0.13f;
    }
    
    float strafe = 0.f, forward = 0.f, up = 0.f;

    if (Inputs.CheckKey(KEY_W)) {
        forward += baseAcceleration;
    }
    if (Inputs.CheckKey(KEY_A)) {
        strafe -= baseAcceleration;
    }
    if (Inputs.CheckKey(KEY_S)) {
        forward -= baseAcceleration;
    }
    if (Inputs.CheckKey(KEY_D)) {
        strafe += baseAcceleration;
    }

    if (Inputs.CheckKey(KEY_SPACE)) {
        up += baseAcceleration;
    }

    MoveRelative(player , strafe, up, forward, friction);*/


}