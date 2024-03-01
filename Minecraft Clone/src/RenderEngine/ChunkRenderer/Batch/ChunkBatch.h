#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "../../../Utils/LogUtils.h"
#include "../../Camera/camera.h"
#include "../../Frustum/frustum.h"
#include "../../../Utils/MathHelper.h"
#include "../../../World/Chunk/Chunk.h"
#include "../../OpenGL/Buffers/Buffer.h"
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

struct DataBufferAddress {
	size_t offset = 0;
	size_t size = 0;
	int x, y, z;
};


class ChunkDrawBatch {
public:

	void SetupBuffers();

	void Reset();

	void GenDrawCommands(int RenderDistance, int VerticalRenderDistance);

	bool AddChunkVertices(std::vector<unsigned int> Data, int x, int y, int z);

	void DeleteChunkVertices(ChunkID ID);

	void SetMaxSize(size_t size);

	void Draw();

	void Bind();

	void Unbind();

	void Cleanup();

	void Defrager(int iterations);

	void UpdateCommandBufferSize();

	Camera* camera;

	std::multimap<size_t, size_t> InsertSpace; // <Slot Size, Index>
	size_t MemoryUsage = 0;
	std::map<size_t, DataBufferAddress> RenderList; //Offset, Render List

	double debugTime = 0.0;
private:

	CFrustum Frustum;
	Buffer VBO, IBO, SSBO;
	VertexArray Array;
	size_t MaxBufferSize = NULL;

	bool UpdateCommands = false;

	std::unordered_map<ChunkID, size_t> RenderListOffsetLookup;
	std::vector<GLint> ChunkShaderPos;

	int lastRenderSize = 0;

	std::unordered_map<ChunkID, std::multimap<size_t, size_t>::iterator> InsertSpaceIteratorsFront;
	std::unordered_map<ChunkID, std::multimap<size_t, size_t>::iterator> InsertSpaceIteratorsBack;

	std::vector<DrawCommandIndirect> DrawCommands;

	size_t AmountOfChunks = 0;
	size_t AmountOfChunkBeingRendered = 0;


};