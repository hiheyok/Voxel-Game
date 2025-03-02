#pragma once
#include "../Level.h"
#include "../Timer/Timer.h"
#include "../../Utils/Clock.h"
#include <thread>
#include "../../Core/Options/Option.h"

struct ServerSettings {
	int gen_thread_count_ = 8;
	int light_engine_thread_count_ = 2;
	int tick_rate_ = 20;
	int horizontal_ticking_distance_ = 10;
	int vertical_ticking_distance_ = 8;
};

//This manages the input/output system of the world
//This also manages the ticking system
class Server {
private:
	Level level_;
	bool stop = true; // TODO: Rename
	std::thread main_server_loop_;
	Timer time; // TODO: Rename
	double mspt_ = 0.0;
public:
	ServerSettings settings_;

	Server() {

	}

	int getChunkCount();

	double getMSPT();// TODO: Rename

	Timer* GetTimer();

	std::vector<EntityProperty> getUpdatedEntities();// TODO: Rename

	std::vector<EntityUUID> getRemovedEntities();// TODO: Rename

	bool CheckEntityOnGround(EntityUUID id);

	void join(Entity& entity);// TODO: Rename

	std::vector<ChunkPos> GetUpdatedChunkPos();

	Chunk* GetChunk(const ChunkPos& ID);

	BlockID GetBlock(const BlockPos& pos);

	bool GetRayIntersection(Ray& ray);

	glm::vec3 GetEntityCollusionTime(EntityUUID entity);

	void StartServer(ServerSettings serverSettings);

	void Stop();

	void Loop();

	void Tick();

	void SendEvent(const Event::Event& pEventIn);
};