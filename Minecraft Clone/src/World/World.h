#pragma once
#ifndef WORLD_H
#define WORLD_H

#include "DataContainer/WorldData.h"
#include "TerrainGeneration/Generator.h"
#include "Event/EventSystem.h"
#include <unordered_set>
#include <vector>
#include "../Utils/Math/Ray/Ray.h"
#include <deque>
#include <mutex>
#include "WorldProperties.h"

struct WorldSettings {
	size_t genThreads = 0;
};

class World : public WorldData {
public:
	void Start(WorldSettings settings);

	void Stop();

	void Tick();
	void Load();
	void QueueEvent(Event event);

	void SetPlayerPos(glm::dvec3 pos);

	void TickUsed(EventID id, int x, int y, int z) {//temp solution
		TickUsage[id].insert(getChunkID(x, y, z));
	}
	bool CheckTickUsed(EventID id, int x, int y, int z) {//temp sol
		return TickUsage[id].contains(getChunkID(x, y, z));
	}

	int H_RenderDistance = 16;
	int V_RenderDistance = 8;

	EventSystem EventManager;

	WorldProperties Properties;

	double MSPT = 0;



private:


	std::unordered_map<int, std::unordered_set<ChunkID>> TickUsage;

	void WorldThread();

	std::thread MainWorldThread;
	std::thread Loader;

	glm::vec3 PlayerPos = glm::vec3(0.f, 0.f, 0.f);

	ChunkGeneration WorldGenerator;

	std::unordered_set<ChunkID> ChunksInQueue;

	std::unordered_set<ChunkID> FIFOOutOfRange;

	bool stop = false;
};

#endif // !WORLD_H