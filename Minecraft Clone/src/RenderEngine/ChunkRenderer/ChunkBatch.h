#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "../../Utils/LogUtils.h"
#include "../Camera/camera.h"
#include "../Frustum/frustum.h"
#include "../../Utils/MathHelper.h"
#include "../../World/Chunk/Chunk.h"
#include "../OpenGL/Buffers/Buffer.h"
#include <unordered_map>
#include <map>

struct DrawCommandIndirect {
	unsigned int  count;
	unsigned int  instanceCount;
	unsigned int  first;
	unsigned int  baseInstance;
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

	void GenDrawCommands(int RenderDistance);

	bool AddChunkVertices(std::vector<unsigned int> Data, bool insertBack, int x, int y, int z);

	void DeleteChunkVertices(ChunkID ID);

	void SetMaxSize(size_t size);

	void Draw();

	void Bind();

	void Unbind();

	void Cleanup();

	void Defrager(int iterations);

	size_t GetRenderObjIndex(size_t offset);

	size_t FindClosestRenderObjIndex(size_t offset);

	Camera* camera;

	std::multimap<size_t, size_t> InsertSpace; // <Slot Size, Index>
	size_t MemoryUsage = 0;
	std::vector<DataBufferAddress> RenderList;

	double debugTime = 0.0;
private:
	
	CFrustum Frustum;
	Buffer VBO, IBO, SSBO;
	VertexArray Array;
	size_t MaxBufferSize = NULL;
	

	bool UpdateCommands = false;

	std::unordered_map<long long int, size_t> RenderListOffsetLookup;
	std::vector<GLint> ChunkShaderPos;

	std::unordered_map<ChunkID, std::multimap<size_t, size_t>::iterator> InsertSpaceIteratorsFront;
	std::unordered_map<ChunkID, std::multimap<size_t, size_t>::iterator> InsertSpaceIteratorsBack;
	
	std::vector<DrawCommandIndirect> DrawCommands;

	

};