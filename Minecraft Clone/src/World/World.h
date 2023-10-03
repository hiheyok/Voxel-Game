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

struct WorldSettings {
	size_t genThreads = 0;
};

class World : public WorldData {
public:
	void Start(WorldSettings settings);

	void Stop();

	void Tick();
	void Load();

	bool RayIntersection(Ray& ray);

	void SetPlayerPos(glm::dvec3 pos);

	glm::dvec3 GetTimeTillCollusion(Entity entity);

	bool IsEntityOnGround(Entity entity);

	int H_RenderDistance = 16;
	int V_RenderDistance = 8;

	int horizontaltickingdistance = 16;
	int vertialtickingdistance = 8;

private:

	

	void WorldThread();
	
	float GetDistanceUntilCollusionSingleDirection(glm::vec3 Origin, int direction, int distancetest);

	std::thread MainWorldThread;
	std::thread Loader;

	glm::vec3 PlayerPos = glm::vec3(0.f,0.f,0.f);

	const float TPS = 20.f;

	Generator WorldGenerator;

	std::unordered_set<ChunkID> ChunksInQueue;

	bool stop = false;
};

#endif // !WORLD_H

