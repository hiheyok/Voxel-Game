#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
struct Ray {
	float Length = 0;

	glm::vec3 Direction = glm::vec3(0., 0., 0.);
	glm::vec3 Origin = glm::vec3(0., 0., 0.);
	glm::vec3 EndPoint = glm::vec3(0., 0., 0.);

	int bouncesurface = 0;
};