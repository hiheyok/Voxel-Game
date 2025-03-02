#pragma once
#include "PlayerPOV.h"
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

	void SetRotation(glm::dvec2 rotation_);

	void SetPosition(glm::dvec3 position);

	void Render();

	void LoadChunkToRenderer(ChunkPos chunk);

	void LoadChunkMultiToRenderer(std::vector<ChunkPos> chunks);

	void Start(GLFWwindow* window_, InternalServer* world_, PerformanceProfiler* pProfilerIn);

	void Stop();

	void Update();

	TerrainRenderer renderer_v2_;
	PerformanceProfiler* profiler_;
private:

	void Worker(int id);

	void TaskScheduler();

	PlayerPOV player_;
	GLFWwindow* window_;
	InternalServer* server_;

	int worker_count_ = NULL;

	bool stop = false;

	std::deque<ChunkPos> task_list_; //All tasks will go in here and they will be distributed to all the workers

	std::deque<std::thread> workers_;
	std::deque<std::deque<ChunkPos>> worker_task_;
	std::deque<std::deque<std::unique_ptr<MeshingV2::ChunkVertexData>>> worker_output_;
	std::deque<std::mutex> worker_locks_;

	std::thread Scheduler;
	std::mutex scheduler_lock_;
};