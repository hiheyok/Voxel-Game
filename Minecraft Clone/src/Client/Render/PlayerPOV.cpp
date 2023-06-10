#include "PlayerPOV.h"

using namespace glm;

void PlayerPOV::SetPosition(vec3 pos) {
	camera.Position = pos;
}

void PlayerPOV::SetRotation(vec2 rot) {
	camera.Yaw = rot.x;
	camera.Pitch = rot.y;

	camera.updateCameraVectors();
}

void PlayerPOV::SetFOV(float FOV) {
	camera.FOV = FOV;
}

Camera* PlayerPOV::getCamera() {
	return &camera;
}
