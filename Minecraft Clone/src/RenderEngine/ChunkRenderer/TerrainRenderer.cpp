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
	for (ChunkDrawBatch& DrawBatch : chunk_solid_batches_) {
		DrawBatch.GenDrawCommands(horizontal_render_distance_, vertical_render_distance_);
	}

	for (ChunkDrawBatch& DrawBatch : chunk_transparent_batches_) {
		DrawBatch.GenDrawCommands(horizontal_render_distance_, vertical_render_distance_);
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
	
	LoadAssets();

	SetupCallSolid();

	for (ChunkDrawBatch& DrawBatch : chunk_solid_batches_) {
		DrawBatch.Bind();
		CubicShader.Use();
		DrawBatch.Draw();
		DrawBatch.Unbind();
	}

	SetupCallTransparent();

	for (ChunkDrawBatch& DrawBatch : chunk_transparent_batches_) {
		DrawBatch.Bind();
		CubicShader.Use();
		DrawBatch.Draw();
		DrawBatch.Unbind();
	}

	
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}

void TerrainRenderer::Defrag(int iterations) {
	for (auto& DrawBatch : chunk_solid_batches_) {
		DrawBatch.Defrager(iterations);
	}

	for (auto& DrawBatch : chunk_solid_batches_) {
		DrawBatch.Defrager(iterations);
	}
}


void TerrainRenderer::Update() {
	int width, height;

	glfwGetWindowSize(window, &width, &height);
	glm::mat4 model = glm::mat4(1.f);

	camera->screen_res_ = glm::vec2(width, height);

	glm::mat4 view = camera->GetViewMatrix();

	int x = width;
	int y = height;
	glm::mat4 projection = glm::perspective(glm::radians(camera->fov_), (float)x / (float)y, 0.1f, 1000000.0f);
	CubicShader.Use();

	float clrMultiplier = 1.4f;

	CubicShader.SetMat4("view", view);
	CubicShader.SetMat4("model", model);
	CubicShader.SetMat4("projection", projection);
	CubicShader.SetFloat("RenderDistance", (float)(horizontal_render_distance_ * 16));
	CubicShader.SetFloat("VerticalRenderDistance", (float)(vertical_render_distance_ * 16));
	CubicShader.SetVec3("camPos", camera->position_);
	CubicShader.SetVec3("tintColor",  glm::vec3(0.40828402 * clrMultiplier, 0.5917159 * clrMultiplier, 0.2781065 * clrMultiplier));
	CubicShader.SetInt("TextureAimIndex", TextureAminationIndex);

	if (time.GetTimePassed_ms() > 100) {
		time.Set();
		TextureAminationIndex++;

		if (TextureAminationIndex == (1 << 15)) {
			TextureAminationIndex = 1;
		}
	}
	

}

void TerrainRenderer::setSettings(uint32_t renderDistance, uint32_t verticalRenderDistance, float fov) {
	horizontal_render_distance_ = renderDistance;
	vertical_render_distance_ = verticalRenderDistance;
	fov_ = fov;
}

void TerrainRenderer::LoadAssets() {
	CubicShader.BindTexture2D(0, g_blocks.block_texture_atlas_.get(), "BlockTexture");
}

void TerrainRenderer::AddChunk(const ChunkPos& pos, const std::vector<uint32_t>& data, std::vector<ChunkDrawBatch>& batchType, FastHashMap<ChunkPos, int>& lookUpMap) {
	if (lookUpMap.count(pos)) {
		size_t BatchIndex = lookUpMap[pos];
		batchType[BatchIndex].DeleteChunkVertices(pos);
		lookUpMap.erase(pos);
	}

	if (data.size() == 0)
		return;

	bool success = false;

	for (int batchIndex = 0; batchIndex < batchType.size(); batchIndex++) {
		size_t meshDataSize = data.size() * sizeof(uint32_t);

		if (batchType[batchIndex].memory_pool_.memory_pool_.FindFreeSpace(meshDataSize) == ULLONG_MAX)
			continue;

		bool InsertSuccess = batchType[batchIndex].AddChunkVertices(data, pos);

		if (!InsertSuccess) continue;

		lookUpMap.emplace(pos, batchIndex);
		success = true;
		break;
	}

	if (!success) {
		g_logger.LogInfo("Terrain Renderer", "Unable to add chunk. Solid buffers are full!");
	}
}

void TerrainRenderer::AddChunk(std::unique_ptr<MeshingV2::ChunkVertexData> MeshData) {
	AddChunk(MeshData->position_, MeshData->solidVertices, chunk_solid_batches_, chunk_batch_solid_lookup_);
	AddChunk(MeshData->position_, MeshData->transparentVertices, chunk_transparent_batches_, chunk_batch_transparent_lookup_);
}


double TerrainRenderer::getDebugTime() {
	double t = 0.0;

	for (int batchIndex = 0; batchIndex < chunk_solid_batches_.size(); batchIndex++) {

		t += chunk_solid_batches_[batchIndex].debug_time_;
	}

	return t;
}

double TerrainRenderer::getFragmentationRate() {
	size_t n = chunk_solid_batches_.size();

	double fragRate = 0;

	for (size_t batchIndex = 0; batchIndex < n; batchIndex++) {

		const auto& batch = chunk_solid_batches_[batchIndex];

		if (batch.render_list_.size() != 0) {

			fragRate += (batch.memory_pool_.statistics_.fragmentation_rate_ / (double)n);
		}


	}

	return 1.0 - fragRate;
}

size_t TerrainRenderer::getVRAMUsageFull() {
	size_t memUsage = 0;

	for (int batchIndex = 0; batchIndex < chunk_solid_batches_.size(); batchIndex++) {

		if (chunk_solid_batches_[batchIndex].render_list_.size() == 0) {
			continue;
		}

		memUsage += chunk_solid_batches_[batchIndex].memory_pool_.statistics_.full_memory_usage_;
	}

	return memUsage;
}

void TerrainRenderer::Cleanup() {
	for (ChunkDrawBatch& batch : chunk_solid_batches_) {
		batch.Cleanup();
	}

	for (ChunkDrawBatch& batch : chunk_transparent_batches_) {
		batch.Cleanup();
	}

	chunk_solid_batches_.clear();
	chunk_batch_solid_lookup_.clear();

	chunk_transparent_batches_.clear();
	chunk_batch_transparent_lookup_.clear();

	stop = true;
}

void TerrainRenderer::SetupShaders() {
	CubicShader.Init("assets/shaders/vert.glsl", "assets/shaders/frag.glsl");
}

void TerrainRenderer::CreateNewSolidBatch() {
	chunk_solid_batches_.emplace_back();
	size_t i = chunk_solid_batches_.size() - 1;
	chunk_solid_batches_[i].SetMaxSize(g_app_options.solid_buffer_size_);
	chunk_solid_batches_[i].SetupBuffers();
	chunk_solid_batches_[i].camera = camera;
}

void TerrainRenderer::CreateNewTransparentBatch() {
	chunk_transparent_batches_.emplace_back();
	size_t i = chunk_transparent_batches_.size() - 1;
	chunk_transparent_batches_[i].SetMaxSize(g_app_options.transparent_buffer_size_);
	chunk_transparent_batches_[i].SetupBuffers();
	chunk_transparent_batches_[i].camera = camera;
}