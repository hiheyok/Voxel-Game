#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "../Camera/camera.h"
#include "../Frustum/frustum.h"
#include "../OpenGL/Shader/Shader.h"
#include "../OpenGL/Texture/Texture.h"
#include "../../Utils/MathHelper.h"
#include "../OpenGL/Buffers/Buffer.h"
#include "Mesh/ChunkMeshing.h"
#include "ChunkBatch.h"
#include <unordered_map>
#include <deque>
#include <queue>



class TerrainRenderer {
public:
	TerrainRenderer() {

	}

	void Initialize(GLFWwindow* window_, Camera* camera_) {
		window = window_;
		camera = camera_;
		SetupShaders();
		LoadAssets();

		CreateNewBatch();
	}

	void PrepareRenderer() {
		for (ChunkDrawBatch& DrawBatch : ChunkBatches) {
			DrawBatch.GenDrawCommands(m_RenderDistance);
		}
	}

	void SetupCall() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CCW);
	}

	void RenderSky() {
		glClearColor(0.46274509803f, 0.65882352941f, 1.0f, 1.0);
	}

	void Render() {

		SetupCall();

		for (ChunkDrawBatch& DrawBatch : ChunkBatches) {
			DrawBatch.Bind();
			SolidShader.use();
			DrawBatch.Draw();
			DrawBatch.Unbind();
		}
	}

	void Defrag(int iterations) {
		for (auto& DrawBatch : ChunkBatches) {
			DrawBatch.Defrager(iterations);
		}
	}

	uint32_t getGapCount() {
		int n = 0;

		for (auto& DrawBatch : ChunkBatches) {
			n += DrawBatch.InsertSpace.size();
		}

		return n;
	}

	void Update() {
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
	}

	void setSettings(uint32_t RenderDistance, float FOV) {
		m_RenderDistance = RenderDistance;
		m_FOV = FOV;
	}

	void LoadAssets() {
		SolidShader.bindTextureArray2D(0,BlockTextureArray.textureID,"BlockTexture");
	}

	void AddChunk(Meshing::ChunkMeshData MeshData) {

		ChunkID id = getChunkID(MeshData.Position);

		if (ChunkBatchLookup.count(id)) {
			size_t BatchIndex = ChunkBatchLookup[id];

			ChunkBatches[BatchIndex].DeleteChunkVertices(id);

			ChunkBatchLookup.erase(id);
		}

		for (int batchIndex = 0; batchIndex < ChunkBatches.size(); batchIndex++) {
			size_t MeshDataSize = MeshData.SolidVertices.size();

			auto it = --ChunkBatches[batchIndex].InsertSpace.end();
			
			if (it->first >= MeshDataSize * sizeof(unsigned int)) {
				ChunkBatches[batchIndex].AddChunkVertices(MeshData.SolidVertices, false, MeshData.Position.x, MeshData.Position.y, MeshData.Position.z);
				ChunkBatchLookup[id] = batchIndex;
				return;
			}
		}

		getLogger()->LogInfo("Terrain Renderer", "Unable to add chunk. Buffers are full!");
	}

	double getDebugTime() {
		double t = 0.0;

		for (int batchIndex = 0; batchIndex < ChunkBatches.size(); batchIndex++) {

			t+=ChunkBatches[batchIndex].debugTime;
		}

		return t;
	}

	double getFragmentationRate() {
		int n = ChunkBatches.size();

		double fragRate = 0;

		for (int batchIndex = 0; batchIndex < n; batchIndex++) {

			auto& batch = ChunkBatches[batchIndex];

			if (batch.RenderList.size() != 0) {
				int index = batch.RenderList.size() - 1;

				size_t FullMemoryUse = batch.RenderList[index].size + batch.RenderList[index].offset; //Include gaps
				size_t MemoryUse = ChunkBatches[batchIndex].MemoryUsage; //Only include vertex data

				fragRate += ((double)MemoryUse / (double)FullMemoryUse) / ((double)n);
			}

			
		}

		return 1.0 - fragRate;
	}

	size_t getVRAMUsageFull() {
		size_t memUsage = 0;

		for (int batchIndex = 0; batchIndex < ChunkBatches.size(); batchIndex++) {

			int index = ChunkBatches[batchIndex].RenderList.size() - 1;

			if (index != -1) {
				memUsage += ChunkBatches[batchIndex].RenderList[index].size + ChunkBatches[batchIndex].RenderList[index].offset;
			}
		}

		return memUsage;
	}

	void Cleanup() {
		for (ChunkDrawBatch& batch : ChunkBatches) {
			batch.Cleanup();
		}

		ChunkBatches.clear();
		ChunkBatchLookup.clear();
	}

private:
	void SetupShaders() {
		SolidShader.init("assets/shaders/vert.glsl", "assets/shaders/frag.glsl");
	}

	void CreateNewBatch() {
		ChunkBatches.emplace_back();
		size_t i = ChunkBatches.size() - 1;
		ChunkBatches[i].SetMaxSize(5000000000);
		ChunkBatches[i].SetupBuffers();
		ChunkBatches[i].camera = camera;
	}

	int m_RenderDistance = 16;
	float m_FOV = 80.f;

	std::vector<ChunkDrawBatch> ChunkBatches;
	std::unordered_map<ChunkID, int> ChunkBatchLookup;

	GLFWwindow* window = nullptr;
	
	Shader SolidShader;
	Camera* camera;
};

