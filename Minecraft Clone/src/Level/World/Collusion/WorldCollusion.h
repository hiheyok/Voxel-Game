#pragma once
#include <glm/vec3.hpp>
#include "../WorldDataAccess.h"
#include "../../../Utils/Math/Ray/Ray.h"
#include "../../Entity/Entity.h"

class WorldCollusionDetector {
private:
	WorldAccess* world;

public:
	void Initialize(WorldAccess* w) {
		world = w;
	}

	bool CheckRayIntersection(Ray& ray);

	float GetDistanceUntilCollusionSingleDirection(glm::vec3 Origin, int direction, int distanceTest);

	glm::dvec3 GetTimeTillCollusion(Entity* entity);

	bool isEntityOnGround(Entity* entity);
};