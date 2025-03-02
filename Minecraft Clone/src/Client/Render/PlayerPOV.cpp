#include "PlayerPOV.h"

using namespace glm;

void PlayerPOV::SetPosition(vec3 pos) {
	camera.position_ = pos;
}

void PlayerPOV::SetRotation(vec2 rot) {
	camera.yaw_ = rot.x;
	camera.pitch_ = rot.y;

	camera.updateCameraVectors();
}

void PlayerPOV::SetFOV(float fov_) {
	camera.fov_ = fov_;
}

Camera* PlayerPOV::getCamera() {
	return &camera;
}
