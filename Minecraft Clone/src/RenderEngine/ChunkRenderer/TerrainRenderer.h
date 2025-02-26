#pragma once
#include  "Mesh/ChunkMeshingV2.h"
#include "../Camera/camera.h"
#include "Batch/ChunkBatch.h"
#include "../OpenGL/Shader/Shader.h"
#include "../../Core/Options/Option.h"
#include "../../Level/Timer/Timer.h"

#include <concurrent_vector.h>
#include <mutex>

class TerrainRenderer {
public:
	TerrainRenderer() {

	}

	void Initialize(GLFWwindow* window_, Camera* camera_);

	void PrepareRenderer();

	void SetupCallSolid();

	void SetupCallTransparent();

	void RenderSky();

	void Render();

	void Defrag(int iterations);

	void Update();

	void setSettings(uint32_t RenderDistance, uint32_t VerticalRenderDistance, float FOV);

	void LoadAssets();

	void AddChunk(MeshingV2::ChunkVertexData* MeshData);

	double getDebugTime();

	double getFragmentationRate();

	size_t getVRAMUsageFull();

	void Cleanup();

	size_t amountOfMeshGenerated = 1;

private:
	void GarbageCollectorThread();

	void AddChunk(const ChunkPos& pos, std::vector<uint32_t> data, std::vector<ChunkDrawBatch>* BatchType, FastHashMap<ChunkPos, int>* LookUpMap);

	void SetupShaders();

	void CreateNewSolidBatch();

	void CreateNewTransparentBatch();

	Concurrency::concurrent_vector<void*> mGarbagePointers{};
	int m_HorizontalRenderDistance = 16;
	int m_VerticalRenderDistance = 16;
	float m_FOV = 80.f;
	int TextureAminationIndex = 0;

	std::vector<ChunkDrawBatch> ChunkSolidBatches;
	FastHashMap<ChunkPos, int> ChunkBatchSolidLookup; //f: ChunkPos -> SolidBatchIndex

	std::vector<ChunkDrawBatch> ChunkTransparentBatches;
	FastHashMap<ChunkPos, int> ChunkBatchTransparentLookup; //f: ChunkPos -> TransparentBatchIndex

	GLFWwindow* window = nullptr;
	bool stop = false;
	std::thread mGarbageCollectorThread;

	Shader CubicShader;
	Camera* camera;
	Timer time;
	std::mutex mLock;
};

