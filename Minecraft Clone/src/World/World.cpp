#include "World.h"

#include "../Utils/Clock.h"
#include <glm/vec3.hpp>
#include <chrono>

using namespace std;
using namespace chrono;
using namespace glm;


bool World::RayIntersection(Ray& ray) {

	vec3 delta = ray.Direction;
	vec3 pos = ray.Origin;
	
	for (int i = 0; i < 100; i++) {

		ivec3 side;
		vec3 l;

		//iterates through the 3 axis
		for (int axis = 0; axis < 3; axis++) {
			//Sets where the ray could intersect to
			if ((pos[axis] == floor(pos[axis])) && (delta[axis] < 0)) {
				side[axis] = -1;
			}
			else {
				side[axis] = delta[axis] >= 0 ? 1 : 0;
			}

			//If delta is 0, it will set the (t) that it will take to insect to some massive value 
			//If delta is not 0, it will set a (t) that will tell us how long it will take the ray to get there 
			
			if (delta[axis] == 0.f) {
				l[axis] = 9999999999999999999.f;
			}
			else {
				l[axis] = ((floor(pos[axis]) + side[axis]) - pos[axis]) / delta[axis];
				//(floor(pos[axis]) + side[axis]) - pos[axis]) is the distance delta is the velocity of the vector 
				//Calculates the time it takes to get to that point for each axis
			}
		}

		float li = 0;

		//Test which axis gives the lowest time 
		if (l.x > l.y) {
			li = l.y > l.z ? l.z : l.y;
		}
		else {
			li = l.x > l.z ? l.z : l.x;
		}

		//Multiply the least time by the vector delta to get the next position
		pos = li * delta + pos;

		vec3 t = pos;

		//If delta is below 0 and the position is on a side of a block, it basicaally means that the pos is going down or behind that block, etc
		//So it subtract 1 block to reflect the position when testing if there's a block there

		for (int axis = 0; axis < 3; axis++) {
			if ((floor(pos[axis]) == pos[axis]) && (delta[axis] < 0))
				t[axis] -= 1;
		}

		//Test if the ray collides if there is a block
		if ((GetBlock((int)floor(t.x), (int)floor(t.y), (int)floor(t.z)) != AIR) && (GetBlock((int)floor(t.x), (int)floor(t.y), (int)floor(t.z)) != NULL_BLOCK)) {

			ray.EndPoint = pos;

			ray.Length = sqrtf(powf(ray.EndPoint.x - ray.Origin.x, 2) + powf(ray.EndPoint.y - ray.Origin.y, 2) + powf(ray.EndPoint.z - ray.Origin.z, 2));

			//Iterates through the axis
			for (int axis = 0; axis < 3; axis++) {
				if ((floor(pos[axis]) == pos[axis]) && (delta[axis] != 0)) { //This test which side the ray collided with the block
					ray.bouncesurface = delta[axis] < 0 ? axis * 2 + 1 : axis * 2; //Set the surface it bounces off. + 1 means that the surface is behind, bottom, etc
					return true;
				}
			}
			return false;
		}
	}
	return false;
}

float World::GetDistanceUntilCollusionSingleDirection(glm::vec3 Origin, int direction, int distancetest) {

	float displacement = 0;

	ivec3 FlooredPos(floor(Origin.x), floor(Origin.y), floor(Origin.z));
	ivec3 Move(0,0,0);

	if (direction % 2 == 0) { //Negative direction has a " + 1". So by taking the modulus, you can find if it is negative or positive. Explaination: PX = 0, NX = 1

		int axis = direction / 2;

		displacement = 1 - (Origin[axis] - (float)FlooredPos[axis]);
		Move[axis] = 1;
	}
	else {
		int axis = (direction - 1) / 2;

		displacement = Origin[axis] - (float)FlooredPos[axis];
		Move[axis] = -1;
	}

	for (int i = 0; i < distancetest; i++) {

		ivec3 Loc = FlooredPos + Move * i;

		if ((GetBlock(Loc.x, Loc.y, Loc.z) != AIR) && (GetBlock(Loc.x, Loc.y, Loc.z) != NULL_BLOCK)) {
			return (float)i + displacement - 1;
		}
	}

	return -1;
}

dvec3 World::GetTimeTillCollusion(Entity entity) {
	AABB Hitbox = EntityTypeList[entity.Type]->GetHitbox();

	vec3 HitboxStart = entity.Position - (Hitbox.size / 2.f);
	vec3 HitboxEnd = entity.Position + (Hitbox.size / 2.f);

	int ix = (int)floor(Hitbox.size.x) + 1;
	int iy = (int)floor(Hitbox.size.y) + 1;
	int iz = (int)floor(Hitbox.size.z) + 1;

	vec3 leasttime(-1.f, -1.f, -1.f);
	
	int SearchDistance = 5;

	float LeastDistance = (float)SearchDistance;

	for (int x = 0; x <= ix; x++) {
		for (int y = 0; y <= iy; y++) {
			for (int z = 0; z <= iz; z++) {

				vec3 origin = HitboxStart + vec3(x, y, z);

				if (origin.x > HitboxEnd.x)
					origin.x = HitboxEnd.x;
				if (origin.y > HitboxEnd.y)
					origin.y = HitboxEnd.y;
				if (origin.z > HitboxEnd.z)
					origin.z = HitboxEnd.z;
				
				bool IsPointOnHitboxSurface = (origin.x == HitboxStart.x) || (origin.x == HitboxEnd.x) || (origin.y == HitboxStart.y) || (origin.y == HitboxEnd.y) || (origin.z == HitboxStart.z) || (origin.z == HitboxEnd.z);

				if (!IsPointOnHitboxSurface) //Checks if the origin is on the surface to optimize stuff
					continue;

				//Iterates through the x, y, and z axis
				for (int axis = 0; axis < 3; axis++) {
					if (entity.Velocity[axis] != 0.f) { //First checks if the velocity isn't 0 because if it is 0, it's not moving in that axis so it's not going to collide in that direction
						int direction = entity.Velocity[axis] < 0 ? axis * 2 + 1 : axis * 2; // The "+1" indicates that the direction is negative 

						float distance = GetDistanceUntilCollusionSingleDirection(origin, direction, (int)floor(LeastDistance) + 2);

						if ((distance < LeastDistance) && (distance != -1.f))
							LeastDistance = distance;

						if (distance != -1.f) { // -1.f means that it cannot find any blocks that could collide in that range (5)
							float time = abs(distance / entity.Velocity[axis]);// This gets the time it takes for the entity to travel that distance

							if (time < leasttime[axis]) {
								leasttime[axis] = time;
							}
							else {
								if (leasttime[axis] == -1.f) { //leasttime[axis] == -1.f means that a "time" value haven't been inputted yet
									leasttime[axis] = time;
								}
							}
						}
					}
				}

			}
		}
	}

	return leasttime;
}
void World::SetPlayerPos(glm::dvec3 pos) {
	PlayerPos = pos;
}

void World::Start() {
	stop = false;

	WorldGenerator.Start(4);

	MainWorldThread = std::thread(&World::WorldThread, this);
}

void World::Stop() {
	stop = true;
}

void World::WorldThread() {
	while (!stop) {
		int ChunksPerTick = 0;
		auto t0 = high_resolution_clock::now();
		
		vec3 pos = PlayerPos / 16.f;

		deque<ChunkID> jobs;

		for (int x = -horizontaltickingdistance; x <= horizontaltickingdistance; x++) {
			for (int y = -0; y <= 8; y++) {
				for (int z = -horizontaltickingdistance; z <= horizontaltickingdistance; z++) {
					int offsetx = (int)floor(pos.x) + x;
					int offsety = y;
					int offsetz = (int)floor(pos.z) + z;

					if (!CheckChunk(offsetx, offsety, offsetz) && (!ChunksInQueue.count(getChunkID(offsetx, offsety, offsetz)))) {
						WorldGenerator.Generate(offsetx, offsety, offsetz);
						ChunksInQueue.insert(getChunkID(offsetx, offsety, offsetz));
					}
				}
			}
		}

		deque<Chunk> GenOutput = WorldGenerator.GetOutput();

		while (!GenOutput.empty()) {

			Chunk chunk = GenOutput.front();
			GenOutput.pop_front();

			SetChunk(chunk);
			ChunksInQueue.erase(chunk.chunkID);

			ChunksPerTick++;
		}

		timerSleepNotPrecise((int)(1000.f / (float)TPS));

		float MSPT = (float)((double)(high_resolution_clock::now() - t0).count() / 1000000.0);

		getLogger()->LogInfo("World", "MSPT: " + std::to_string(MSPT) + " | Chunks Per Second: " + std::to_string((float)ChunksPerTick / (MSPT / 1000)));
	}
}

bool World::IsEntityOnGround(Entity entity) {
	AABB Hitbox = EntityTypeList[entity.Type]->GetHitbox();

	vec3 HitboxStart = entity.Position - (Hitbox.size / 2.f);
	vec3 HitboxEnd = entity.Position + (Hitbox.size / 2.f);

	int ix = (int)floor(Hitbox.size.x) + 1;
	int iz = (int)floor(Hitbox.size.z) + 1;

	int SearchDistance = 5;

	float LeastLength = (float)SearchDistance;

	float OnGroundError = 0.01f;

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