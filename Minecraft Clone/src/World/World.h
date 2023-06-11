#pragma once
#ifndef WORLD_H
#define WORLD_H

#include "DataContainer/ChunkMapData.h"
#include <concurrent_unordered_map.h>
#include <concurrent_queue.h>
#include <concurrent_priority_queue.h>
#include <unordered_set>
#include <vector>
#include "Entity/Entity.h"
#include "../Utils/Math/Ray/Ray.h"
#include <deque>
#include <mutex>

class World {
public:
	void Start();

	void Stop();

	bool RayIntersection(Ray& ray);

	void SetBlock(BlockID block, int x, int y, int z);

	void SetPlayerPos(glm::dvec3 pos);

	void CreateChunk(int x, int y, int z);

	glm::dvec3 GetTimeTillCollusion(Entity entity);

	concurrency::concurrent_queue<Chunk> ChunksUpdated;

	bool IsEntityOnGround(Entity entity);

	ChunkMap Chunks;

	bool ChunksUpdatedWriting = false;
	bool PauseChunksUpdatedWriting = false;

private:

	void WorldThread();

	void LoaderThread();

	void ChunkGenerationWorker(int id);
	
	float GetDistanceUntilCollusionSingleDirection(glm::vec3 Origin, int direction, int distancetest);

	void PauseWorker(int WorkerID);

	void UnpauseWorker(int WorkerID);

	void PauseLoader();

	void UnpauseLoader();


	std::thread MainWorldThread;
	std::thread Loader;

	bool LoaderPause = false;
	bool LoaderIsWorking = false;

	std::vector<std::thread> ChunkGenerationWorkers;
	std::vector<concurrency::concurrent_priority_queue<ChunkID>> ChunkWorkerJob;
	std::vector<concurrency::concurrent_queue<Chunk>> WorkerChunkOutput;
	std::vector<bool> WorkerIsWorking;
	std::vector<bool> WorkerPause;

	const int ChunkWorkerCount = 2;
	int WorkerSelect = 0;

	int ChunksPerTick = 0;

	bool stop = false;

	glm::vec3 PlayerPos = glm::vec3(0.f,0.f,0.f);

	const float TPS = 20.f;

	const int horizontaltickingdistance = 8;
	const int vertialtickingdistance = 4;

	concurrency::concurrent_queue<ChunkID> ChunkJobQueue;

	std::unordered_set<ChunkID> ChunksInQueue;

	FastNoiseLite noise;
};

#endif // !WORLD_H

