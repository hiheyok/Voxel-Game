#pragma once

#include "Mesh/ChunkMeshing.h"
#include "../Camera/camera.h"
#include "Batch/ChunkBatch.h"
#include "../OpenGL/Shader/Shader.h"

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

	uint32_t getGapCount();

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
	void SetupShaders();

	void CreateNewSolidBatch();

	void CreateNewTransparentBatch();

	bool UpdateDrawCommands = false;

	int m_HorizontalRenderDistance = 16;
	int m_VerticalRenderDistance = 16;
	float m_FOV = 80.f;

	std::vector<ChunkDrawBatch> ChunkSolidBatches;
	std::unordered_map<ChunkID, int> ChunkBatchSolidLookup;

	std::vector<ChunkDrawBatch> ChunkTransparentBatches;
	std::unordered_map<ChunkID, int> ChunkBatchTransparentLookup;

	GLFWwindow* window = nullptr;

	Shader SolidShader;
	Shader TransparentShader;
	Camera* camera;
};

