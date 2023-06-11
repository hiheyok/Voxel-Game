#include "World.h"

#include "../Utils/Clock.h"
#include <glm/vec3.hpp>
#include <chrono>

using namespace std;
using namespace chrono;
using namespace glm;

void World::SetBlock(BlockID block, int x, int y, int z) {

	int c[3]{ floor((float)x / 16.f) ,floor((float)y / 16.f) ,floor((float)z / 16.f) }; //c[3] is the position of the chunk

	if (!Chunks.CheckChunk(c[0],c[1],c[2])) {
		Chunk chunk;
		chunk.SetPosition(c[0], c[1], c[2]);
		Chunks.InsertChunk(chunk, c[0], c[1], c[2]);
	}

	Chunks.ChangeBlockGlobal(block, x, y, z);

	ChunksUpdated.push(Chunks.GetChunk(getChunkID(c[0], c[1], c[2])));

	for (int axis = 0; axis < 3; axis++) {
		for (int face = 0; face < 2; face++) {
			int q[3]{ c[0],c[1],c[2] }; //q is the position of the neighbor

			q[axis] += (-2 * face) + 1;

			if (Chunks.CheckChunk(q[0], q[1], q[2])) {
				ChunksUpdated.push(Chunks.GetChunk(getChunkID(q[0], q[1], q[2])));
			}
		}
	}
}

bool World::RayIntersection(Ray& ray) {

	vec3 delta = ray.Direction;
	vec3 pos = ray.Origin;
	
	for (int i = 0; i < 100; i++) {

		ivec3 side;
		vec3 l;
		vec3 TravelDistance;

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
			TravelDistance[axis] = powf((floor(pos[axis]) + side[axis]) - pos[axis], 2) + powf(l[axis] * delta[(axis + 1) % 3], 2) + powf(l[axis] * delta[(axis + 2) % 3], 2);
			//Using that time, it calculates the total distance the vector will travel within that time for each axis
		}

		float li = 0;

		//Test which axis gives the lowest time 
		if (TravelDistance.x > TravelDistance.y) {
			li = TravelDistance.y > TravelDistance.z ? l.z : l.y;
		}
		else {
			li = TravelDistance.x > TravelDistance.z ? l.z : l.x;
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
		if ((Chunks.GetBlockGlobal(floor(t.x), floor(t.y), floor(t.z)) != AIR) && (Chunks.GetBlockGlobal(floor(t.x), floor(t.y), floor(t.z)) != NULL_BLOCK)) {

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

		if ((Chunks.GetBlockGlobal(Loc.x, Loc.y, Loc.z) != AIR) && (Chunks.GetBlockGlobal(Loc.x, Loc.y, Loc.z) != NULL_BLOCK)) {
			return (float)i + displacement - 1;
		}
	}

	return -1;
}

dvec3 World::GetTimeTillCollusion(Entity entity) {
	AABB Hitbox = EntityTypeList[entity.Type]->GetHitbox();

	vec3 HitboxStart = entity.Position - (Hitbox.size / 2.f);
	vec3 HitboxEnd = entity.Position + (Hitbox.size / 2.f);

	int ix = floor(Hitbox.size.x) + 1;
	int iy = floor(Hitbox.size.y) + 1;
	int iz = floor(Hitbox.size.z) + 1;

	vec3 leasttime(-1.f, -1.f, -1.f);
	
	int SearchDistance = 5;

	float LeastDistance = SearchDistance;

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

						float distance = GetDistanceUntilCollusionSingleDirection(origin, direction, floor(LeastDistance) + 2);

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

	ChunkGenerationWorkers.resize(ChunkWorkerCount);
	WorkerChunkOutput.resize(ChunkWorkerCount);
	ChunkWorkerJob.resize(ChunkWorkerCount);
	WorkerIsWorking.resize(ChunkWorkerCount);
	WorkerPause.resize(ChunkWorkerCount);

	MainWorldThread = std::thread(&World::WorldThread, this);

	for (int i = 0; i < ChunkWorkerCount; i++) {
		ChunkGenerationWorkers[i] = thread(&World::ChunkGenerationWorker, this, i);
	}
	Loader = thread(&World::LoaderThread, this);
}

void World::Stop() {
	stop = true;
}

void World::PauseWorker(int WorkerID) {
	WorkerPause[WorkerID] = true;
	while (WorkerIsWorking[WorkerID]) {}
}

void World::UnpauseWorker(int WorkerID) {
	WorkerPause[WorkerID] = false;
}

void World::PauseLoader() {
	LoaderPause = true;
	while (LoaderIsWorking) {}
}

void World::UnpauseLoader() {
	LoaderPause = false;
}

void World::WorldThread() {
	while (!stop) {
		ChunksPerTick = 0;
		auto t0 = high_resolution_clock::now();
		
		vec3 pos = PlayerPos / 16.f;

		deque<ChunkID> jobs;

		for (int x = -horizontaltickingdistance; x <= horizontaltickingdistance; x++) {
			for (int y = -0; y <= 8; y++) {
				for (int z = -horizontaltickingdistance; z <= horizontaltickingdistance; z++) {
					int offsetx = floor(pos.x) + x;
					int offsety =  y;
					int offsetz = floor(pos.z) + z;

					if (!Chunks.CheckChunk(offsetx, offsety, offsetz) && (!ChunksInQueue.count(getChunkID(offsetx, offsety, offsetz)))) {
						jobs.push_back(getChunkID(offsetx, offsety, offsetz));
						ChunksInQueue.insert(getChunkID(offsetx, offsety, offsetz));
					}
				}
			}
		}

		if (jobs.size() != 0) {
			PauseLoader();

			while (jobs.size() != 0) {
				ChunkJobQueue.push(jobs.back());
				jobs.pop_back();
			}

			UnpauseLoader();
		}

		for (int WorkerID = 0; WorkerID < ChunkWorkerCount; WorkerID++) {

			if (!WorkerChunkOutput[WorkerID].empty()) {

				PauseWorker(WorkerID);

				while (PauseChunksUpdatedWriting) {}

				ChunksUpdatedWriting = true;
				while (!WorkerChunkOutput[WorkerID].empty()) {
					Chunk chunk;
					if (WorkerChunkOutput[WorkerID].try_pop(chunk)) {
						Chunks.InsertChunk(chunk, chunk.Position.x, chunk.Position.y, chunk.Position.z);
						ChunksInQueue.erase(chunk.chunkID);
						ChunksUpdated.push(chunk);
						
					}


				}
				ChunksUpdatedWriting = false;
				UnpauseWorker(WorkerID);

			}

			UnpauseWorker(WorkerID);
		}

		timerSleepNotPrecise(1000.f / TPS);

		float MSPT = ((double)(high_resolution_clock::now() - t0).count() / 1000000);

		//getLogger()->LogInfo("World", "MSPT: " + std::to_string(MSPT) + " | Chunks Per Second: " + std::to_string((float)ChunksPerTick / (MSPT / 1000)));
	}
}


void World::ChunkGenerationWorker(int id) {
	const int WorkerID = id;

	while (!stop) {
		if (!WorkerPause[WorkerID]) {
			while (!ChunkWorkerJob[WorkerID].empty()) {
				WorkerIsWorking[WorkerID] = true;

				ChunkID chunkid;

				if (ChunkWorkerJob[WorkerID].try_pop(chunkid)) {
					Chunk chunk;

					ivec3 Pos = ChunkIDToPOS(chunkid);

					chunk.SetPosition(Pos.x, Pos.y, Pos.z);
					chunk.Generate(&noise);

					WorkerChunkOutput[WorkerID].push(chunk);
					ChunksPerTick++;
					WorkerIsWorking[WorkerID] = false;
				}

				
				if (WorkerPause[WorkerID])
					break;
			}
		}
		timerSleepNotPrecise(5);
	}


}

void World::CreateChunk(int x, int y, int z) {
	if (!ChunksInQueue.count(getChunkID(x, y, z))) {
		ChunkJobQueue.push(getChunkID(x, y, z));
		ChunksInQueue.insert(getChunkID(x, y, z));
	}
}

void World::LoaderThread() {
	while (!stop)
	{
		if (!LoaderPause) {
			LoaderIsWorking = true;

			while (!ChunkJobQueue.empty()) {
				ChunkID job;
				if (ChunkJobQueue.try_pop(job)) {
					if ((WorkerSelect % ChunkWorkerCount) == 0)
						WorkerSelect = 0;

					ChunkWorkerJob[WorkerSelect].push(job);

					WorkerSelect++;
				}

				if (LoaderPause)
					break;
			}

			LoaderIsWorking = false;

		}
		timerSleepNotPrecise(5);
	} 
}

bool World::IsEntityOnGround(Entity entity) {
	AABB Hitbox = EntityTypeList[entity.Type]->GetHitbox();

	vec3 HitboxStart = entity.Position - (Hitbox.size / 2.f);
	vec3 HitboxEnd = entity.Position + (Hitbox.size / 2.f);

	int ix = floor(Hitbox.size.x) + 1;
	int iz = floor(Hitbox.size.z) + 1;

	int SearchDistance = 5;

	float LeastLength = SearchDistance;

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
			Distance = GetDistanceUntilCollusionSingleDirection(origin, NY, floor(LeastLength) + 2);

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