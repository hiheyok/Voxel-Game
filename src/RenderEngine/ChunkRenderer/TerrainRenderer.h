#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Mesh/Mesher.h"

#include "../Camera/camera.h"
#include "../frustum/frustum.h"
#include "../OpenGL/shader/shader.h"
#include "../OpenGL/Texture/texture.h"
#include "../../Utils/MathHelper.h"
#include "../../Utils/MutithreadedData.h"
#include "../../Minecraft/Core/Registry.h"
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
	}

	void PrepareRenderer() {
		for (ChunkDrawBatch& DrawBatch : ChunkBatches) {
			DrawBatch.GenDrawCommands(RenderDistance);
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

	}

	void LoadAssets() {

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

	int RenderDistance = 16;

	std::vector<ChunkDrawBatch> ChunkBatches;
	std::unordered_map<CHUNK_ID, int> ChunkBatchLookup;

	GLFWwindow* window = nullptr;
	
	Shader* SolidShader;
};

