#pragma once

#include "Mesh/ChunkMeshing.h"
#include "../Camera/camera.h"
#include "Batch/ChunkBatch.h"
#include "../OpenGL/Shader/Shader.h"
#include "../../Core/Options/Option.h"
#include "../../World/Server/Time/Timer.h"

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

	void AddChunk(Meshing::ChunkMeshData* MeshData);

	double getDebugTime();

	double getFragmentationRate();

	size_t getVRAMUsageFull();

	void Cleanup();

	size_t amountOfMeshGenerated = 1;

private:
	void AddChunk(ChunkID ID, std::vector<uint32_t> data, std::vector<ChunkDrawBatch>* BatchType, std::unordered_map<ChunkID, int>* LookUpMap);

	void SetupShaders();

	void CreateNewSolidBatch();

	void CreateNewTransparentBatch();

	int m_HorizontalRenderDistance = 16;
	int m_VerticalRenderDistance = 16;
	float m_FOV = 80.f;
	int TextureAminationIndex = 0;

	std::vector<ChunkDrawBatch> ChunkSolidBatches;
	std::unordered_map<ChunkID, int> ChunkBatchSolidLookup; //f: ChunkID -> SolidBatchIndex

	std::vector<ChunkDrawBatch> ChunkTransparentBatches;
	std::unordered_map<ChunkID, int> ChunkBatchTransparentLookup; //f: ChunkID -> TransparentBatchIndex

	GLFWwindow* window = nullptr;

	Shader CubicShader;
	Camera* camera;
	Timer time;
};

