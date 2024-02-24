#include "World.h"

#include "../Utils/Clock.h"

#include <glm/vec3.hpp>
#include <chrono>
#include <glm/geometric.hpp>
#include "Event/EventHandler.h"
#include "Server/Time/Timer.h"


using namespace std;
using namespace chrono;
using namespace glm;

void World::SetPlayerPos(glm::dvec3 pos) {
	PlayerPos = pos;
}

void World::Start(WorldSettings settings) {

	Block::WorldPTR = (void*)this;

	WorldGenerator.Start(settings.genThreads);

	MainWorldThread = std::thread(&World::WorldThread, this);
}

void World::Stop() {
	WorldGenerator.Stop();
}

void World::WorldThread() {
	Logger.LogInfo("World","Shutting down main world thread");
}

void World::Tick() {
	Timer time;

	int QueueSize = EventManager.getSize();

	for (int i = 0; i < QueueSize; i++) {
		Event e = EventManager.GetNextEvent();
		if (e.Type == NULL_EVENT)
			break;
		if (e.Type == BLOCK_EVENT) {
			if (CheckTickUsed(e.Data.BlockEvent.id, e.Data.BlockEvent.x, e.Data.BlockEvent.y, e.Data.BlockEvent.z) && (e.Data.BlockEvent.BypassLimit == 0)) {
				continue;
			}
			TickUsed(e.Data.BlockEvent.id, e.Data.BlockEvent.x, e.Data.BlockEvent.y, e.Data.BlockEvent.z);
		}

		EventHandler.ExecuteEvent(e);
	}

	TickUsage.clear();

	//Tick all entities

	EntityData.Tick();

	std::unordered_map<EntityUUID, Entity> UpdatedEntities = EntityData.ClientGetEntityUpdate();
	std::unordered_set<EntityUUID> Removed = EntityData.getRemovedEntities();

	EntityUpdateLock.lock();
	for (auto& e : UpdatedEntities) {
		EntityUpdated[e.first] = e.second;
	}
	for (auto& e : Removed) {
		RemovedEntity.insert(e);
	}
	EntityUpdateLock.unlock();

	MSPT = time.GetTimePassed_ms();
	
}

void World::QueueEvent(Event event) {
	EventManager.AddEvent(event);
}

void World::Load() {

//	Timer time;

	vec3 pos = PlayerPos / 16.f;

	ivec3 InitalPos((int)pos.x, (int)pos.y, (int)pos.z);

	deque<ivec3> FIFO;

	int OutOfRangeSize = FIFOOutOfRange.size();

	for (const auto& element : FIFOOutOfRange) {
		ivec3 ChunkPos = ChunkIDToPOS(element);

		FIFO.push_back(ChunkPos);
	}

	FIFO.push_back(InitalPos);

	int MaxIndex = 0; 

	while (!FIFO.empty()) { 
	
		ivec3 ChunkPos = FIFO.front();
		FIFO.pop_front();

		ivec3 Offset = InitalPos - ChunkPos;

		if (Offset.x * Offset.x + Offset.z * Offset.z > H_RenderDistance * H_RenderDistance) {
			if (OutOfRangeSize == 0) {
				FIFOOutOfRange.insert(getChunkID(ChunkPos));
			}
			else {
				OutOfRangeSize--;
			}
			continue;
		}

		if (abs(Offset.y) > V_RenderDistance) {
			if (OutOfRangeSize == 0) {
				FIFOOutOfRange.insert(getChunkID(ChunkPos));
			}
			else {
				OutOfRangeSize--;
			}
			continue;
		}

		if (CheckChunk(ChunkPos.x, ChunkPos.y, ChunkPos.z) || ChunksInQueue.count(getChunkID(ChunkPos.x, ChunkPos.y, ChunkPos.z))) {
			if (OutOfRangeSize != 0) {
				OutOfRangeSize--;
			}
			continue;
		}

		if (OutOfRangeSize != 0) {
			FIFOOutOfRange.erase(getChunkID(ChunkPos));
			OutOfRangeSize--;
		}

		WorldGenerator.Generate(ChunkPos.x, ChunkPos.y, ChunkPos.z);
		ChunksInQueue.insert(getChunkID(ChunkPos.x, ChunkPos.y, ChunkPos.z));

		for (int side = 0; side < 6; side++) {

			ivec3 newObj = ChunkPos;

			newObj[side % 3] += ((side & 0b1) << 1) - 1;

			FIFO.push_back(newObj);
		}
	}

	deque<Chunk*> GenOutput = WorldGenerator.GetOutput();

	while (!GenOutput.empty()) {
		Chunk* chunk = GenOutput.front();
		GenOutput.pop_front();

		SetChunk(chunk);
		ChunksInQueue.erase(chunk->chunkID);
	}
}
