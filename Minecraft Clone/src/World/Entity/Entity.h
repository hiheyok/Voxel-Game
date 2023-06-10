#pragma once
#include "Entities.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Entity {
	glm::vec3 Velocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 Position = glm::vec3(0.f, 0.f, 0.f);
	glm::vec2 Rotation = glm::vec2(0.f, 0.f);

	EntityID Type = NULL;
};