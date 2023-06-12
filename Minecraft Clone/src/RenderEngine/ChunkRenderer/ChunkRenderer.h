#ifndef CHUNK_R_H_
#define CHUNK_R_H_
#define _CRTDBG_MAP_ALLOC
#include "Mesh/Mesher.h"

#include "../Camera/camera.h"
#include "../frustum/frustum.h"
#include "../OpenGL/shader/shader.h"
#include "../OpenGL/Texture/texture.h"
#include "../../Utils/MathHelper.h"
#include "../../Utils/MutithreadedData.h"
#include "Mesh/ChunkMeshing.h"
#include <unordered_map>
#include <deque>
#include <queue>

struct DrawArraysIndirectCommand {
	unsigned int  count;
	unsigned int  instanceCount;
	unsigned int  first;
	unsigned int  baseInstance;
};

struct ChunkRenderDataBufferAddress {
	size_t offset = 0;
	size_t size = 0;
	int x = 0;
	int y = 0;
	int z = 0;
};


class ChunkRenderer {

public:

	void init(GLFWwindow* window_, Camera* camera_) {

		window = window_;
		camera = camera_;
		SolidShader = new Shader;
		SolidShader->init("assets/shaders/vert.glsl", "assets/shaders/frag.glsl");

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);
		glGenBuffers(1, &SSBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, GPUBufferSizeSolid, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, GPUBufferSizeSolid / 500, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)(0 * sizeof(unsigned int)));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)(1 * sizeof(unsigned int)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, GPUSSBOMAXSIZE, nullptr, GL_DYNAMIC_COPY);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void _DeleteChunk(long long int ChunkID) {
		if (ChunkRenderListSolidOffsetLookup.count(ChunkID)) {
			size_t index = GetRenderSolidList(ChunkRenderListSolidOffsetLookup[ChunkID]);
			GPUMemoryUsage -= ChunkRenderListSolid[index].size;
			MeshList[ChunkID] = false;
			MeshList.erase(ChunkID);
			ChunkRenderListSolid.erase(ChunkRenderListSolid.begin() + index);
			ChunkRenderListSolidOffsetLookup.erase(ChunkID);
		}
	}

	size_t GetRenderSolidList(size_t offset) {
		//binary search thing

		size_t low = 0;
		size_t high = ChunkRenderListSolid.size() - 1;

		while (true) {
			size_t mid = (size_t)((low + high) * 0.5);
			if (offset == ChunkRenderListSolid[mid].offset) {
				return mid;
			}
			else if (offset > ChunkRenderListSolid[mid].offset) {
				low = mid + 1;
			}
			else {
				high = mid - 1;
			}
			if (low == high) {
				break;
			}
		}

		if (offset == ChunkRenderListSolid[high].offset) {
			return high;
		}

		return -1;

	}



	void _AddChunk(ChunkMesh data) {

		unsigned long long int ChunkID = getChunkID(data.chunk->Position);

		size_t MeshSizeSolid = data.vertices.size() * sizeof(unsigned int);

		if (MeshSizeSolid == 0) {
			MeshList[ChunkID] = true;
			return;
		}

		ChunkRenderDataBufferAddress renderdata;
		renderdata.x = data.chunk->Position.x;
		renderdata.y = data.chunk->Position.y;
		renderdata.z = data.chunk->Position.z;

		if (ChunkRenderListSolid.size() == 0) {
			renderdata.offset = 0;
			renderdata.size = MeshSizeSolid;
			GPUMemoryUsage += MeshSizeSolid;
			insertData(VBO, GL_ARRAY_BUFFER, renderdata.offset, &data.vertices);
			ChunkRenderListSolid.insert(ChunkRenderListSolid.begin(), renderdata);
			MeshList[ChunkID] = true;
			ChunkRenderListSolidOffsetLookup[ChunkID] = 0;
			UpdateDrawCommands = true;
		}
		else {
			if (ChunkRenderListSolid.back().offset + ChunkRenderListSolid.back().size + MeshSizeSolid < GPUBufferSizeSolid) {
				renderdata.offset = ChunkRenderListSolid.back().offset + ChunkRenderListSolid.back().size;
				renderdata.size = MeshSizeSolid;
				GPUMemoryUsage += MeshSizeSolid;
				insertData(VBO, GL_ARRAY_BUFFER, renderdata.offset, &data.vertices);
				ChunkRenderListSolid.emplace_back(renderdata);
				MeshList[ChunkID] = true;
				ChunkRenderListSolidOffsetLookup[ChunkID] = renderdata.offset;
				UpdateDrawCommands = true;
			}
			else {
				bool added = false;
				for (int i = 0; i < ChunkRenderListSolid.size(); i++) {
					if (ChunkRenderListSolid[i].offset + ChunkRenderListSolid[i].size + MeshSizeSolid < ChunkRenderListSolid[i + 1].offset) {
						renderdata.offset = ChunkRenderListSolid[i].offset + ChunkRenderListSolid[i].size;
						renderdata.size = MeshSizeSolid;
						GPUMemoryUsage += MeshSizeSolid;
						insertData(VBO, GL_ARRAY_BUFFER, renderdata.offset, &data.vertices);
						ChunkRenderListSolid.insert(ChunkRenderListSolid.begin() + i + 1, renderdata);
						MeshList[ChunkID] = true;
						ChunkRenderListSolidOffsetLookup[ChunkID] = renderdata.offset;
						added = true;
						UpdateDrawCommands = true;
						break;
					}
				}
				if (added == false) {
					getLogger()->LogError("Chunk Renderer", "GPU Out of Memory: " + std::to_string(GPUMemoryUsage));
				}
			}
		}
	}

	void GenCallDrawCommands() {
		DrawArraysIndirectCommandListSolid.clear();
		SolidChunkShaderPos.clear();
		glm::ivec3 Pos = glm::ivec3(floor(camera->Position.x / 16), floor(camera->Position.y / 16), floor(camera->Position.z / 16));

		fr.CalculateFrustum(camera);

		int SolidIndex = 1;
		for (int i = 0; i < ChunkRenderListSolid.size(); i++) {
			ChunkRenderDataBufferAddress data = ChunkRenderListSolid[(ChunkRenderListSolid.size() - 1) - i];

			if (FindDistanceNoSqrt(data.x, data.y, data.z, Pos.x, Pos.y, Pos.z) < pow(RenderDistance, 2)) {//if (!(data.x + -Pos.x > RenderDistance || data.y + -Pos.y > RenderDistance || data.z + -Pos.z > RenderDistance || data.x + -Pos.x < -RenderDistance || data.y + -Pos.y < -RenderDistance || data.z + -Pos.z < -RenderDistance)) {//if (FindDistance(data.second.x, data.second.y, data.second.z, (int)x12 / CHUNK_SIZE, (int)y12 / CHUNK_SIZE, (int)z12 / CHUNK_SIZE) <= renderDistance) {
				if (fr.SphereInFrustum((float)data.x * 16, (float)data.y * 16, (float)data.z * 16, (float)32)) {
					DrawArraysIndirectCommand cmd;
					cmd.count = (unsigned int)data.size / (sizeof(unsigned int) * 2);
					cmd.instanceCount = 1;
					cmd.first = (unsigned int)data.offset / (sizeof(unsigned int) * 2);
					cmd.baseInstance = SolidIndex;
					DrawArraysIndirectCommandListSolid.push_back(cmd);
					SolidChunkShaderPos.emplace_back(data.x);
					SolidChunkShaderPos.emplace_back(data.y);
					SolidChunkShaderPos.emplace_back(data.z);
					SolidIndex++;
				}
			}

		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, SolidChunkShaderPos.size() * sizeof(int), SolidChunkShaderPos.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
		glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, DrawArraysIndirectCommandListSolid.size() * sizeof(DrawArraysIndirectCommand), DrawArraysIndirectCommandListSolid.data());
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}

	void insertData(GLuint buffer, GLenum target, size_t offset, std::vector<unsigned int>* data) {
		glBindBuffer(target, buffer);
		glBufferSubData(target, offset, data->size() * sizeof(unsigned int), data->data());
		glBindBuffer(target, 0);
	}

	void draw() {
		glClearColor(0.46274509803f, 0.65882352941f, 1.0f, 1.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		//	glDepthFunc(GL_ALWAYS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CCW);

		glBindVertexArray(VAO);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO);
		SolidShader->use();
		//getLogger()->LogDebug("Renderer", std::to_string(DrawArraysIndirectCommandListSolid.size()));
		glMultiDrawArraysIndirect(GL_TRIANGLES, (GLvoid*)0, (GLsizei)DrawArraysIndirectCommandListSolid.size(), 0);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		//	
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}

	void UpdateData() {
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
		SolidShader->setFloat("RenderDistance", (float)(RenderDistance * 16));
		SolidShader->setVec3("camPos", camera->Position);
	}

	void AddChunkMesh(ChunkMesh& chunk) {
		
		if (MeshList.count(getChunkID(chunk.chunk->Position))) {
			_DeleteChunk(getChunkID(chunk.chunk->Position));
			_AddChunk(chunk);
		}
		else {
			_AddChunk(chunk);
		}
	}

	void ReloadAssets() {

		SolidShader->bindTextureArray2D(0, BlockTextureArray.textureID, "BlockTexture");
	}

	std::unordered_map<long long int, bool> MeshList;


	//Settings

	int RenderDistance = 64;

private:

	static int FastFloor(double f) { return f >= 0 ? (int)f : (int)f - 1; }

	bool UpdateDrawCommands = false;

	std::vector<glm::vec3> SpreadCache;
	int SpreadCacheRenderDistance = 0;
	int SpreadCacheRenderHeight = 0;

	std::deque<int> FIBO;
	std::vector<int> spread;

	GLuint VBO, VAO, IBO, SSBO = 0;
	GLuint VBOT, VAOT, IBOT = 0;

	GLuint BlockTexture = 0;

	Shader* SolidShader = nullptr;

	std::vector<GLint> SolidChunkShaderPos;
	std::vector<GLint> TransparentChunkShaderPos;

	size_t GPUMemoryUsage = 0;

	size_t GPUBufferSizeSolid = 1000000000;
	size_t GPUBufferSizeTransparent = 120000000;
	size_t GPUSSBOMAXSIZE = 10000000;

	GLFWwindow* window = nullptr;
	Camera* camera = nullptr;
	CFrustum fr;

	std::unordered_map<ChunkID, ChunkMesh> MeshDataQueue;

	std::unordered_map<long long int, size_t> ChunkRenderListSolidOffsetLookup;
	std::unordered_map<long long int, size_t> ChunkRenderListTransparentOffsetCache;

	std::vector<ChunkRenderDataBufferAddress> ChunkRenderListSolid;
	std::vector<DrawArraysIndirectCommand> DrawArraysIndirectCommandListSolid;

	std::vector<ChunkRenderDataBufferAddress> ChunkRenderListTransparent;
	std::vector<DrawArraysIndirectCommand> DrawArraysIndirectCommandListTransparent;
};
#endif