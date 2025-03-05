#pragma once
#include "PlayerPOV.h"
#include "../../RenderEngine/ChunkRenderer/Mesh/ChunkMeshingV2.h"
#include "../../Level/Server/Communication/InternalServer.h"
#include "../../RenderEngine/ChunkRenderer/TerrainRenderer.h"
#include "../../Utils/ThreadPool.h"
#include "WorldRenderInfo.h"
#include "../Profiler/PerformanceProfiler.h"

class WorldRender : public WorldRenderInfo{
public:

	void SetRotation(glm::dvec2 rotation);

	void SetPosition(glm::dvec3 position);

	void Render();

	void LoadChunkToRenderer(ChunkPos chunk);

	void LoadChunkMultiToRenderer(std::vector<ChunkPos> chunks);

	void Start(GLFWwindow* window, InternalServer* server, PerformanceProfiler* profiler);

	void Stop();

	void Update();

	TerrainRenderer renderer_v2_;
	PerformanceProfiler* profiler_;
private:
	static std::unique_ptr<Mesh::ChunkVertexData> Worker(const ChunkPos& pos);
	
	std::unique_ptr<ThreadPool<ChunkPos, std::unique_ptr<Mesh::ChunkVertexData>, WorldRender::Worker>> mesh_thread_pool_;
	std::vector<std::unique_ptr<Mesh::ChunkVertexData>> mesh_add_queue_;

	PlayerPOV player_;
	GLFWwindow* window_;
	static InternalServer* server_;
};

inline InternalServer* WorldRender::server_ = nullptr;