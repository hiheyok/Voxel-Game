#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "../Camera/camera.h"
#include "../frustum/frustum.h"
#include "../OpenGL/shader/shader.h"
#include "../OpenGL/Texture/texture.h"
#include "../../Utils/MathHelper.h"
#include "../../Utils/MutithreadedData.h"
#include "../OpenGL/Buffers/Buffer.h"
#include "ChunkBatch.h"
#include <unordered_map>
#include <deque>
#include <queue>



class TerrainRenderer {
public:
	TerrainRenderer() {

	}

	void Initialize(GLFWwindow* window_) {
		window = window_;
		SetupShaders();
		LoadAssets();
	}

	void PrepareRenderer() {
		for (ChunkDrawBatch& DrawBatch : ChunkBatches) {
			DrawBatch.GenDrawCommands(m_RenderDistance);
		}
	}

	void Render() {
		for (ChunkDrawBatch& DrawBatch : ChunkBatches) {
			DrawBatch.Bind();
			SolidShader->use();
			DrawBatch.Draw();
			DrawBatch.Unbind();
		}
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
		SolidShader->use();

		SolidShader->setMat4("view", view);
		SolidShader->setMat4("model", model);
		SolidShader->setMat4("projection", projection);
		SolidShader->setFloat("RenderDistance", (float)(m_RenderDistance * 16));
		SolidShader->setVec3("camPos", camera->Position);
	}

	void setSettings(uint32_t RenderDistance, float FOV) {
		m_RenderDistance = RenderDistance;
		m_FOV = FOV;
	}

	void LoadAssets() {
		SolidShader->bindTextureArray2D(0,BlockTextureArray.textureID,"BlockTexture");
	}

	void AddChunk(Chunk chunk) {

	}
private:
	void SetupShaders() {
		SolidShader->init("assets/shaders/vert.glsl", "assets/shaders/frag.glsl");
	}

	void CreateNewBatch() {
		ChunkDrawBatch Batch;
		Batch.SetMaxSize(10000000);
		Batch.SetupBuffers();
	}

	int m_RenderDistance = 16;
	float m_FOV = 80.f;

	std::vector<ChunkDrawBatch> ChunkBatches;
	std::unordered_map<ChunkID, int> ChunkBatchLookup;

	GLFWwindow* window = nullptr;
	
	Shader* SolidShader = nullptr;
	Camera* camera;
};

