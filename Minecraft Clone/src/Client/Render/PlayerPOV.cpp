#include "PlayerPOV.h"

using namespace glm;

void PlayerPOV::SetPosition(vec3 pos) {
	camera_.position_ = pos;
}

void PlayerPOV::SetRotation(vec2 rot) {
	camera_.yaw_ = rot.x;
	camera_.pitch_ = rot.y;

	camera_.updateCameraVectors();
}

void PlayerPOV::SetFOV(float fov) {
	camera_.fov_ = fov;
}

Camera* PlayerPOV::GetCamera() {
	return &camera_;
}
