#pragma once
#include <glm/vec3.hpp>
#include "../WorldDataAccess.h"
#include "../../../Utils/Math/Ray/Ray.h"
#include "../../Entity/Entity.h"

class WorldCollusionDetector {
private:
	WorldAccess* world_;

public:
	void Initialize(WorldAccess* w) {
		world_ = w;
	}

	bool CheckRayIntersection(Ray& ray);

	float GetDistanceUntilCollusionSingleDirection(glm::vec3 Origin, int direction, int distanceTest);

	glm::dvec3 GetTimeTillCollusion(Entity* entity);

	bool isEntityOnGround(Entity* entity);
};