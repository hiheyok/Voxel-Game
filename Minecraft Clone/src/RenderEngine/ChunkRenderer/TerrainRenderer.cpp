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

	time.Set();

	CreateNewSolidBatch();
	CreateNewTransparentBatch();
}

void TerrainRenderer::PrepareRenderer() {
	for (ChunkDrawBatch& DrawBatch : ChunkSolidBatches) {
		DrawBatch.GenDrawCommands(m_HorizontalRenderDistance, m_VerticalRenderDistance);
	}

	for (ChunkDrawBatch& DrawBatch : ChunkTransparentBatches) {
		DrawBatch.GenDrawCommands(m_HorizontalRenderDistance, m_VerticalRenderDistance);
	}
}

void TerrainRenderer::SetupCallSolid() {
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
}

void TerrainRenderer::SetupCallTransparent() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
}

void TerrainRenderer::RenderSky() {
	glClearColor(0.46274509803f, 0.65882352941f, 1.0f, 1.0);
}

void TerrainRenderer::Render() {
	
	

	SetupCallSolid();

	for (ChunkDrawBatch& DrawBatch : ChunkSolidBatches) {
		DrawBatch.Bind();
		CubicShader.use();
		DrawBatch.Draw();
		DrawBatch.Unbind();
	}

	SetupCallTransparent();

	for (ChunkDrawBatch& DrawBatch : ChunkTransparentBatches) {
		DrawBatch.Bind();
		CubicShader.use();
		DrawBatch.Draw();
		DrawBatch.Unbind();
	}

	
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}

void TerrainRenderer::Defrag(int iterations) {
	for (auto& DrawBatch : ChunkSolidBatches) {
		DrawBatch.Defrager(iterations);
	}

	for (auto& DrawBatch : ChunkSolidBatches) {
		DrawBatch.Defrager(iterations);
	}
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
	CubicShader.use();

	CubicShader.setMat4("view", view);
	CubicShader.setMat4("model", model);
	CubicShader.setMat4("projection", projection);
	CubicShader.setFloat("RenderDistance", (float)(m_HorizontalRenderDistance * 16));
	CubicShader.setFloat("VerticalRenderDistance", (float)(m_VerticalRenderDistance * 16));
	CubicShader.setVec3("camPos", camera->Position);
	CubicShader.setInt("TextureAimIndex", TextureAminationIndex);

	if (time.GetTimePassed_ms() > 100) {
		time.Set();
		TextureAminationIndex++;

		if (TextureAminationIndex == (1 << 15)) {
			TextureAminationIndex = 1;
		}
	}
	

}

void TerrainRenderer::setSettings(uint32_t RenderDistance, uint32_t VerticalRenderDistance, float FOV) {
	m_HorizontalRenderDistance = RenderDistance;
	m_VerticalRenderDistance = VerticalRenderDistance;
	m_FOV = FOV;
}

void TerrainRenderer::LoadAssets() {
	CubicShader.bindTextureArray2D(0, Blocks.BlockTextureArray.textureID, "BlockTexture");
}

void TerrainRenderer::AddChunk(ChunkID ID, std::vector<uint32_t> data, std::vector<ChunkDrawBatch>* BatchType, std::unordered_map<ChunkID, int>* LookUpMap) {
	if (LookUpMap->count(ID)) {
		size_t BatchIndex = LookUpMap->at(ID);
		BatchType->at(BatchIndex).DeleteChunkVertices(ID);
		LookUpMap->erase(ID);
	}

	if (data.size() == 0)
		return;

	glm::ivec3 ChunkPos = ChunkIDToPOS(ID);

	bool Success = false;

	for (int batchIndex = 0; batchIndex < BatchType->size(); batchIndex++) {
		size_t MeshDataSize = data.size() * sizeof(uint32_t);

		if (BatchType->at(batchIndex).MemoryPool.MemoryPool.FindFreeSpace(MeshDataSize) == ULLONG_MAX)
			continue;

		bool InsertSuccess = BatchType->at(batchIndex).AddChunkVertices(data, ChunkPos.x, ChunkPos.y, ChunkPos.z);

		if (!InsertSuccess) continue;

		LookUpMap->emplace(std::pair<ChunkID, int>(ID, batchIndex));
		Success = true;
		break;
		
	}

	if (!Success) {
		Logger.LogInfo("Terrain Renderer", "Unable to add chunk. Solid buffers are full!");
	}
}

void TerrainRenderer::AddChunk(MeshingV2::ChunkMeshData* MeshData) {
	ChunkID ID = getChunkID(MeshData->Position);
	AddChunk(ID, MeshData->Vertices, &ChunkSolidBatches, &ChunkBatchSolidLookup);
	AddChunk(ID, MeshData->TransparentVertices, &ChunkTransparentBatches, &ChunkBatchTransparentLookup);
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

			fragRate += (batch.MemoryPool.Statistics.FragmentationRate / (double)n);
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

		memUsage += ChunkSolidBatches[batchIndex].MemoryPool.Statistics.FullMemoryUsage;
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
	CubicShader.init("assets/shaders/vert.glsl", "assets/shaders/frag.glsl");
}

void TerrainRenderer::CreateNewSolidBatch() {
	ChunkSolidBatches.emplace_back();
	size_t i = ChunkSolidBatches.size() - 1;
	ChunkSolidBatches[i].SetMaxSize(AppOptions.SolidBufferSize);
	ChunkSolidBatches[i].SetupBuffers();
	ChunkSolidBatches[i].camera = camera;
}

void TerrainRenderer::CreateNewTransparentBatch() {
	ChunkTransparentBatches.emplace_back();
	size_t i = ChunkTransparentBatches.size() - 1;
	ChunkTransparentBatches[i].SetMaxSize(AppOptions.TransparentBufferSize);
	ChunkTransparentBatches[i].SetupBuffers();
	ChunkTransparentBatches[i].camera = camera;
}