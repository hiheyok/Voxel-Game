#pragma once
#ifndef WORLD_H
#define WORLD_H

#include "DataContainer/WorldData.h"
#include "TerrainGeneration/Generator.h"
#include <concurrent_unordered_map.h>
#include <concurrent_queue.h>
#include <concurrent_priority_queue.h>
#include <unordered_set>
#include <vector>
#include "../Utils/Math/Ray/Ray.h"
#include <deque>
#include <mutex>

class World : public WorldData {
public:
	void Start();

	void Stop();

	bool RayIntersection(Ray& ray);

	void SetPlayerPos(glm::dvec3 pos);

	glm::dvec3 GetTimeTillCollusion(Entity entity);

	bool IsEntityOnGround(Entity entity);

private:

	void WorldThread();
	
	float GetDistanceUntilCollusionSingleDirection(glm::vec3 Origin, int direction, int distancetest);

	std::thread MainWorldThread;

	std::thread Loader;

	bool stop = false;

	glm::vec3 PlayerPos = glm::vec3(0.f,0.f,0.f);

	const float TPS = 20.f;

	const int horizontaltickingdistance = 16;
	const int vertialtickingdistance = 4;

	Generator WorldGenerator;

	std::unordered_set<ChunkID> ChunksInQueue;

};

#endif // !WORLD_H

