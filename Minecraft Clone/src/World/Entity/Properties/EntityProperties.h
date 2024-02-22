#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct EntityProperty {
	glm::vec3 Velocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 Position = glm::vec3(0.f, 0.f, 0.f);
	glm::vec2 Rotation = glm::vec2(0.f, 0.f);
};