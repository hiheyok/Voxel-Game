#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "RenderCommandBuffer.h"
#include "../../../Utils/LogUtils.h"
#include "../../Camera/camera.h"
#include "../../Frustum/frustum.h"
#include "../../../Utils/MathHelper.h"
#include "../../../Level/Chunk/Chunk.h"
#include "../../OpenGL/Buffers/Buffer.h"
#include "../MemoryPool/ChunkMemoryPool.h"
#include <unordered_map>
#include <list>
#include <map>
#include <array>

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

	Camera* camera;

	std::map<size_t, ChunkMemoryPoolOffset> RenderList; //f: Offset -> RenderInfo

	double debugTime = 0.0;

	ChunkGPUMemoryPool MemoryPool;
private:
	ChunkDrawCommandBuffer CommandBuffer;
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