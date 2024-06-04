#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "../../Typenames.h"

struct EntityProperty {
	glm::vec3 Velocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 Position = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 Acceleration = glm::vec3(0.f, 0.f, 0.f);
	glm::vec2 Rotation = glm::vec2(0.f, 0.f);

	float MaxSpeed = 50;

	bool onGround = false;

	bool isChunkLoader = false;

	EntityUUID EntityUUID = 0x00000000;
	EntityTypeID Type = NULL; //State what type of entity is it. Zombie? Human??
};