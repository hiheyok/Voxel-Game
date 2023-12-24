#include "TerrainRenderer.h"


#include "../Frustum/frustum.h"

#include "../OpenGL/Texture/Texture.h"
#include "../../Utils/MathHelper.h"
#include "../OpenGL/Buffers/Buffer.h"


#include <unordered_map>
#include <deque>
#include <queue>


void TerrainRenderer::Initialize(GLFWwindow* window_, Camera* camera_) {
	window = window_;
	camera = camera_;
	SetupShaders();
	LoadAssets();

	CreateNewSolidBatch();
	CreateNewTransparentBatch();
}

void TerrainRenderer::PrepareRenderer() {
	for (ChunkDrawBatch& DrawBatch : ChunkSolidBatches) {
		DrawBatch.GenDrawCommands(m_RenderDistance);
	}

	for (ChunkDrawBatch& DrawBatch : ChunkTransparentBatches) {
		DrawBatch.GenDrawCommands(m_RenderDistance);
	}
}

void TerrainRenderer::SetupCallSolid() {
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
}

void TerrainRenderer::SetupCallTransparent() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void TerrainRenderer::RenderSky() {
	glClearColor(0.46274509803f, 0.65882352941f, 1.0f, 1.0);
}

void TerrainRenderer::Render() {
	SetupCallSolid();

	for (ChunkDrawBatch& DrawBatch : ChunkSolidBatches) {
		DrawBatch.Bind();
		SolidShader.use();
		DrawBatch.Draw();
		DrawBatch.Unbind();
	}

	SetupCallTransparent();

	for (ChunkDrawBatch& DrawBatch : ChunkTransparentBatches) {
		DrawBatch.Bind();
		TransparentShader.use();
		DrawBatch.Draw();
		DrawBatch.Unbind();
	}
}

void TerrainRenderer::Defrag(int iterations) {
	for (auto& DrawBatch : ChunkSolidBatches) {
		DrawBatch.Defrager(iterations);
	}

	for (auto& DrawBatch : ChunkSolidBatches) {
		DrawBatch.Defrager(iterations);
	}
}

uint32_t TerrainRenderer::getGapCount() {
	int n = 0;

	for (auto& DrawBatch : ChunkSolidBatches) {
		n += DrawBatch.InsertSpace.size();
	}

	for (auto& DrawBatch : ChunkTransparentBatches) {
		n += DrawBatch.InsertSpace.size();
	}

	return n;
}

void TerrainRenderer::Update() {
	int width, height;

	glfwGetWindowSize(window, &width, &height);
	glm::mat4 model = glm::mat4(1.f);

	camera->screenRes = glm::vec2(width, height);

	glm::mat4 view = camera->GetViewMatrix();

	int x = width;
	int y = height;
	glm::mat4 projection = glm::perspective(glm::radians(camera->FOV), (float)x / (float)y, 0.1f, 1000000.0f);
	SolidShader.use();

	SolidShader.setMat4("view", view);
	SolidShader.setMat4("model", model);
	SolidShader.setMat4("projection", projection);
	SolidShader.setFloat("RenderDistance", (float)(m_RenderDistance * 16));
	SolidShader.setVec3("camPos", camera->Position);

	TransparentShader.use();

	TransparentShader.setMat4("view", view);
	TransparentShader.setMat4("model", model);
	TransparentShader.setMat4("projection", projection);
	TransparentShader.setFloat("RenderDistance", (float)(m_RenderDistance * 16));
	TransparentShader.setVec3("camPos", camera->Position);
}

void TerrainRenderer::setSettings(uint32_t RenderDistance, float FOV) {
	m_RenderDistance = RenderDistance;
	m_FOV = FOV;
}

void TerrainRenderer::LoadAssets() {
	SolidShader.bindTextureArray2D(0, Blocks.BlockTextureArray.textureID, "BlockTexture");
}

void TerrainRenderer::AddChunk(Meshing::ChunkMeshData* MeshData) {
	ChunkID id = getChunkID(MeshData->Position);

	if (ChunkBatchSolidLookup.count(id)) {
		size_t BatchIndex = ChunkBatchSolidLookup[id];

		ChunkSolidBatches[BatchIndex].DeleteChunkVertices(id);

		ChunkBatchSolidLookup.erase(id);
	}

	UpdateDrawCommands = true;

	if (MeshData->SolidVertices.size() == 0) {
	}
	else {
		bool success = false;

		for (int batchIndex = 0; batchIndex < ChunkSolidBatches.size(); batchIndex++) {
			size_t MeshDataSize = MeshData->SolidVertices.size();

			size_t size = (--(ChunkSolidBatches[batchIndex].InsertSpace.end()))->first;

			if (size >= MeshDataSize * sizeof(unsigned int)) {
				ChunkSolidBatches[batchIndex].AddChunkVertices(MeshData->SolidVertices, MeshData->Position.x, MeshData->Position.y, MeshData->Position.z);
				ChunkBatchSolidLookup[id] = batchIndex;
				success = true;
				break;
			}
		}

		if (!success) {
			Logger.LogInfo("Terrain Renderer", "Unable to add chunk. Solid buffers are full!");
		}
	}


	if (ChunkBatchTransparentLookup.count(id)) {
		size_t BatchIndex = ChunkBatchTransparentLookup[id];

		ChunkTransparentBatches[BatchIndex].DeleteChunkVertices(id);

		ChunkBatchTransparentLookup.erase(id);
	}

	UpdateDrawCommands = true;

	if (MeshData->TransparentVertices.size() != 0) {
		bool success = false;

		for (int batchIndex = 0; batchIndex < ChunkTransparentBatches.size(); batchIndex++) {
			size_t MeshDataSize = MeshData->TransparentVertices.size();

			size_t size = (--(ChunkTransparentBatches[batchIndex].InsertSpace.end()))->first;

			if (size >= MeshDataSize * sizeof(unsigned int)) {
				ChunkTransparentBatches[batchIndex].AddChunkVertices(MeshData->TransparentVertices, MeshData->Position.x, MeshData->Position.y, MeshData->Position.z);
				ChunkBatchTransparentLookup[id] = batchIndex;
				success = true;
				break;
			}
		}

		if (!success) {
			Logger.LogInfo("Terrain Renderer", "Unable to add chunk. Transparent buffers are full!");
		}

	}

	delete MeshData;
}


double TerrainRenderer::getDebugTime() {
	double t = 0.0;

	for (int batchIndex = 0; batchIndex < ChunkSolidBatches.size(); batchIndex++) {

		t += ChunkSolidBatches[batchIndex].debugTime;
	}

	return t;
}

double TerrainRenderer::getFragmentationRate() {
	int n = ChunkSolidBatches.size();

	double fragRate = 0;

	for (int batchIndex = 0; batchIndex < n; batchIndex++) {

		auto& batch = ChunkSolidBatches[batchIndex];

		if (batch.RenderList.size() != 0) {

			size_t FullMemoryUse = (--batch.RenderList.end())->second.size + (--batch.RenderList.end())->second.offset; //Include gaps
			size_t MemoryUse = ChunkSolidBatches[batchIndex].MemoryUsage; //Only include vertex data

			fragRate += ((double)MemoryUse / (double)FullMemoryUse) / ((double)n);
		}


	}

	return 1.0 - fragRate;
}

size_t TerrainRenderer::getVRAMUsageFull() {
	size_t memUsage = 0;

	for (int batchIndex = 0; batchIndex < ChunkSolidBatches.size(); batchIndex++) {

		if (ChunkSolidBatches[batchIndex].RenderList.size() == 0) {
			continue;
		}

		memUsage += (--ChunkSolidBatches[batchIndex].RenderList.end())->second.size + (--ChunkSolidBatches[batchIndex].RenderList.end())->second.offset;
	}

	return memUsage;
}

void TerrainRenderer::Cleanup() {
	for (ChunkDrawBatch& batch : ChunkSolidBatches) {
		batch.Cleanup();
	}

	for (ChunkDrawBatch& batch : ChunkTransparentBatches) {
		batch.Cleanup();
	}

	ChunkSolidBatches.clear();
	ChunkBatchSolidLookup.clear();

	ChunkTransparentBatches.clear();
	ChunkBatchTransparentLookup.clear();
}

void TerrainRenderer::SetupShaders() {
	SolidShader.init("assets/shaders/vert.glsl", "assets/shaders/frag.glsl");
	TransparentShader.init("assets/shaders/TransparentChunkVert.glsl", "assets/shaders/TransparentChunkFrag.glsl");
}

void TerrainRenderer::CreateNewSolidBatch() {
	ChunkSolidBatches.emplace_back();
	size_t i = ChunkSolidBatches.size() - 1;
	ChunkSolidBatches[i].SetMaxSize(3000000000);
	ChunkSolidBatches[i].SetupBuffers();
	ChunkSolidBatches[i].camera = camera;
}

void TerrainRenderer::CreateNewTransparentBatch() {
	ChunkTransparentBatches.emplace_back();
	size_t i = ChunkTransparentBatches.size() - 1;
	ChunkTransparentBatches[i].SetMaxSize(3000000000);
	ChunkTransparentBatches[i].SetupBuffers();
	ChunkTransparentBatches[i].camera = camera;
}