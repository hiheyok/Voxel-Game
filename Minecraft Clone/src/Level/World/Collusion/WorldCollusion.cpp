#include "WorldCollusion.h"
#include "../../../Utils/Math/vectorOperations.h"
#include "../../Entity/Entities.h"

using namespace glm;

float WorldCollusionDetector::GetDistanceUntilCollusionSingleDirection(vec3 Origin, int direction, int distanceTest) {
	float displacement = 0;

	ivec3 FlooredPos(floor(Origin.x), floor(Origin.y), floor(Origin.z));
	int Move = 0;

	int axis = direction >> 1;

	if (direction & 0b1) {
		displacement = Origin[axis] - (float)FlooredPos[axis];
		Move = -1;
	}
	else {
		displacement = 1 - (Origin[axis] - (float)FlooredPos[axis]);
		Move = 1;
	}

	for (int i = 0; i < distanceTest; i++) {

		FlooredPos[axis] += Move * (int)(i != 0);

		BlockID b = world->getBlock(FlooredPos.x, FlooredPos.y, FlooredPos.z);

		if (Blocks.getBlockType(b)->Properties->isSolid) {
			return (float)i + displacement - 1.f;
		}
	}

	return -1.f;
}

dvec3 WorldCollusionDetector::GetTimeTillCollusion(Entity* entity) {
	AABB Hitbox = EntityList.GetEntity(entity->Properties.Type)->GetHitbox();

	vec3 HitboxStart = entity->Properties.Position - (Hitbox.size / 2.f);
	vec3 HitboxEnd = entity->Properties.Position + (Hitbox.size / 2.f);

	int ix = (int)floor(Hitbox.size.x) + 1;
	int iy = (int)floor(Hitbox.size.y) + 1;
	int iz = (int)floor(Hitbox.size.z) + 1;

	ivec3 i_bound = ivec3(ix, iy, iz);

	vec3 leasttime(-1.f, -1.f, -1.f);

	int SearchDistance = 5;


	float LeastDistance = (float)SearchDistance;

	for (int axis = 0; axis < 3; axis++) {

		//0 = x; 1 = y; 2 = z

		int u_axis = (axis + 1) % 3;
		int v_axis = (axis + 2) % 3;

		for (int u = 0; u <= i_bound[u_axis]; u++) {
			for (int v = 0; v <= i_bound[v_axis]; v++) {
				ivec3 offset(0, 0, 0);

				offset[axis] = i_bound[axis] * (entity->Properties.Velocity[axis] >= 0);
				offset[u_axis] = u;
				offset[v_axis] = v;

				vec3 origin = HitboxStart + (vec3)offset;

				if (origin.x > HitboxEnd.x)
					origin.x = HitboxEnd.x;
				if (origin.y > HitboxEnd.y)
					origin.y = HitboxEnd.y;
				if (origin.z > HitboxEnd.z)
					origin.z = HitboxEnd.z;

				bool IsPointOnHitboxSurface =
					(origin.x == HitboxStart.x) ||
					(origin.x == HitboxEnd.x) ||
					(origin.y == HitboxStart.y) ||
					(origin.y == HitboxEnd.y) ||
					(origin.z == HitboxStart.z) ||
					(origin.z == HitboxEnd.z);

				if (!IsPointOnHitboxSurface) //Checks if the origin is on the surface to optimize stuff
					continue;

				if (entity->Properties.Velocity[axis] == 0.f) //First checks if the velocity isn't 0 because if it is 0, it's not moving in that axis so it's not going to collide in that direction
					continue;

				int direction = axis * 2 + (entity->Properties.Velocity[axis] < 0); // The "+1" indicates that the direction is negative 

				float distance = GetDistanceUntilCollusionSingleDirection(origin, direction, (int)floor(LeastDistance) + 2);

				if ((distance < LeastDistance) && (distance != -1.f))
					LeastDistance = distance;

				if (distance == -1.f) // -1.f means that it cannot find any blocks that could collide in that range (5)
					continue;

				float time = abs(distance / entity->Properties.Velocity[axis]);// This gets the time it takes for the entity to travel that distance

				if (time < leasttime[axis]) {
					leasttime[axis] = time;
					continue;
				}

				if (leasttime[axis] == -1.f) //leasttime[axis] == -1.f means that a "time" value haven't been inputted yet
					leasttime[axis] = time;
			}
		}
	}

	return leasttime;
}

bool WorldCollusionDetector::CheckRayIntersection(Ray& ray) {

	//Direction with magnitude
	vec3 Delta = ray.Direction;

	//Direction to Step
	ivec3 Direction = Sign(Delta);

	//Location in grid
	ivec3 BlockPos = ivec3(floor(ray.Origin.x), floor(ray.Origin.y), floor(ray.Origin.z));

	//To keep track of point location
	vec3 EndPoint = ray.Origin;

	vec3 DeltaDist(
		abs(1 / Delta[0]), abs(1 / Delta[1]), abs(1 / Delta[2])
	);

	//Stepping Variable
	vec3 sideDist(
		((float)Direction[0] * ((float)BlockPos[0] - EndPoint[0]) + ((float)Direction[0] * 0.5f) + 0.5f) * DeltaDist[0],
		((float)Direction[1] * ((float)BlockPos[1] - EndPoint[1]) + ((float)Direction[1] * 0.5f) + 0.5f) * DeltaDist[1],
		((float)Direction[2] * ((float)BlockPos[2] - EndPoint[2]) + ((float)Direction[2] * 0.5f) + 0.5f) * DeltaDist[2]
	);

	//Max Iterations
	const uint32_t max_iterations = 50;
	uint32_t iterations = 0;

	bvec3 mask(false, false, false);

	while (iterations < max_iterations) {
		iterations++;
		
		BlockID b = world->getBlock(BlockPos.x, BlockPos.y, BlockPos.z);

		if (Blocks.getBlockType(b)->Properties->isSolid) {

			ray.EndPoint = (vec3)BlockPos;

			ray.Length = sqrtf(powf(ray.EndPoint.x - ray.Origin.x, 2) + powf(ray.EndPoint.y - ray.Origin.y, 2) + powf(ray.EndPoint.z - ray.Origin.z, 2));

			for (int axis = 0; axis < 3; axis++) {
				if (mask[axis]) {
					ray.bouncesurface = Delta[axis] < 0 ? axis * 2 + 1 : axis * 2; //Set the surface it bounces off. + 1 means that the surface is behind, bottom, etc
					return true;
				}
			}
			return false;
		}

		bvec3 l1 = lessThan(sideDist[0], sideDist[1], sideDist[2], sideDist[1], sideDist[2], sideDist[0]);
		bvec3 l2 = lessThanEqual(sideDist[0], sideDist[1], sideDist[2], sideDist[2], sideDist[0], sideDist[1]);

		mask[0] = l1[0] && l2[0];
		mask[1] = l1[1] && l2[1];
		mask[2] = l1[2] && l2[2];

		sideDist[0] += (float)mask[0] * DeltaDist[0];
		sideDist[1] += (float)mask[1] * DeltaDist[1];
		sideDist[2] += (float)mask[2] * DeltaDist[2];

		BlockPos[0] += ((int)mask[0]) * Direction[0];
		BlockPos[1] += ((int)mask[1]) * Direction[1];
		BlockPos[2] += ((int)mask[2]) * Direction[2];
	}

	return false;
}

bool WorldCollusionDetector::isEntityOnGround(Entity* entity) {
	AABB Hitbox = EntityList.GetEntity(entity->Properties.Type)->GetHitbox();

	vec3 HitboxStart = entity->Properties.Position - (Hitbox.size / 2.f);
	vec3 HitboxEnd = entity->Properties.Position + (Hitbox.size / 2.f);

	int ix = (int)floor(Hitbox.size.x) + 1;
	int iz = (int)floor(Hitbox.size.z) + 1;

	int SearchDistance = 5;

	float LeastLength = (float)SearchDistance;

	float OnGroundError = 0.001f;

	if (entity->Properties.Velocity.y > 0.f) {
		return false;
	}

	for (int x = 0; x <= ix; x++) {
		for (int z = 0; z <= iz; z++) {
			vec3 origin = HitboxStart + vec3(x, 0, z);

			if (origin.x > HitboxEnd.x)
				origin.x = HitboxEnd.x;
			if (origin.z > HitboxEnd.z)
				origin.z = HitboxEnd.z;

			float Distance = 0.f;

			//Set the distance to check to the previose least length from collusion to optimize searching
			Distance = GetDistanceUntilCollusionSingleDirection(origin, NY, (int)floor(LeastLength) + 2);

			if (Distance < LeastLength) {
				LeastLength = Distance;
			}

			if ((Distance != -1) && (Distance < OnGroundError)) {
				return true;
			}

		}
	}
	return false;
}