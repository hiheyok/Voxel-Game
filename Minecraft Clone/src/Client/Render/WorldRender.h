#pragma once
#include "PlayerPOV.h"
#include "../../RenderEngine/ChunkRenderer/Mesh/ChunkMeshing.h"
#include "../../RenderEngine/ChunkRenderer/Mesh/ChunkMeshingV2.h"
#include <concurrent_unordered_map.h>
#include <concurrent_queue.h>
#include <thread>
#include <mutex>
#include "../../Level/Server/Communication/InternalServer.h"
#include "../../RenderEngine/ChunkRenderer/TerrainRenderer.h"
#include "WorldRenderInfo.h"
#include "../Profiler/PerformanceProfiler.h"

class WorldRender : public WorldRenderInfo{
public:

	void SetRotation(glm::dvec2 rotation);

	void SetPosition(glm::dvec3 position);

	void Render();

	void LoadChunkToRenderer(ChunkID chunk);

	void LoadChunkMultiToRenderer(std::vector<ChunkID> chunks);

	void Start(GLFWwindow* window_, InternalServer* world_, PerformanceProfiler* pProfilerIn);

	void Stop();

	void Update();

	TerrainRenderer RendererV2;
	PerformanceProfiler* mProfiler;
private:

	void Worker(int id);

	void TaskScheduler();

	PlayerPOV player;
	GLFWwindow* window;
	InternalServer* server;

	int WorkerCount = NULL;

	bool stop = false;

	std::deque<ChunkID> TaskList; //All tasks will go in here and they will be distributed to all the workers

	std::deque<std::thread> Workers;
	std::deque<std::deque<ChunkID>> WorkerTask;
	std::deque<std::deque<MeshingV2::ChunkVertexData*>> WorkerOutput;
	std::deque<std::mutex> WorkerLocks;

	std::thread Scheduler;
	std::mutex SchedulerLock;
};