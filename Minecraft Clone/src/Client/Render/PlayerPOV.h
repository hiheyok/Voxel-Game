#pragma once
#include "../../RenderEngine/Camera/camera.h"


class PlayerPOV {
public:
	void SetPosition(glm::vec3 pos);
	void SetRotation(glm::vec2 rot);
	void SetFOV(float FOV);


	Camera* getCamera();
private:
	Camera camera;
};