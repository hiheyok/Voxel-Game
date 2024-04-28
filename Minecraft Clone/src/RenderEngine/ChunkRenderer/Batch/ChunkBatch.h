#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "../../../Utils/LogUtils.h"
#include "../../Camera/camera.h"
#include "../../Frustum/frustum.h"
#include "../../../Utils/MathHelper.h"
#include "../../../World/Chunk/Chunk.h"
#include "../../OpenGL/Buffers/Buffer.h"
#include "../../ChunkRendererV2/MemoryPool/ChunkMemoryPool.h"
#include <unordered_map>
#include <list>
#include <map>
#include <array>


struct DrawCommandIndirect {
	unsigned int  count;
	unsigned int  instanceCount;
	unsigned int  first;
	unsigned int  baseInstance;

	DrawCommandIndirect() {

	}

	DrawCommandIndirect(uint32_t count_, uint32_t instanceCount_, uint32_t first_, uint32_t baseInstance_) : count(count_), instanceCount(instanceCount_), first(first_), baseInstance(baseInstance_) {

	}

	void set(uint32_t count_, uint32_t instanceCount_, uint32_t first_, uint32_t baseInstance_) {
		count = count_;
		instanceCount = instanceCount_;
		first = first_;
		baseInstance = baseInstance_;
	}
};

class ChunkDrawBatch {
public:

	void SetupBuffers();

	void Reset();

	void GenDrawCommands(int RenderDistance, int VerticalRenderDistance);

	bool AddChunkVertices(std::vector<uint32_t>& Data, int x, int y, int z);

	void DeleteChunkVertices(ChunkID ID);

	void SetMaxSize(size_t size);

	void Draw();

	void Bind();

	void Unbind();

	void Cleanup();

	void Defrager(int iterations);

	void UpdateCommandBufferSize();

	void ErrorCheck();

	void GetDebugLogs() {
		Logger.LogDebug("Chunk Batch", "Debug Logs:\n" + Logs);
	}

	Camera* camera;

	std::map<size_t, ChunkMemoryPoolOffset> RenderList; //f: Offset -> RenderInfo

	double debugTime = 0.0;

	ChunkGPUMemoryPool MemoryPool;
private:

	std::string Logs = "";

	CFrustum Frustum;
	Buffer IBO, SSBO;
	VertexArray Array;
	size_t MaxBufferSize = NULL;

	bool UpdateCommands = false;

	std::unordered_map<ChunkID, size_t> RenderListOffsetLookup; //f: ChunkID  -> Offset
	std::vector<GLint> ChunkShaderPos;

	std::vector<DrawCommandIndirect> DrawCommands;

	size_t AmountOfChunks = 0;
	size_t AmountOfChunkBeingRendered = 0;

	Buffer TransferBuffer;
};