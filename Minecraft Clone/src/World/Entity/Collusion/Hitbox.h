#pragma once
#include <glm/vec3.hpp>

struct AABB {
	glm::vec3 size = glm::dvec3(0.f,0.f,0.f);
	glm::vec3 offset = glm::dvec3(0.f,0.f,0.f);
};