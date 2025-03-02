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
#include <list>
#include <map>
#include <array>

class ChunkDrawBatch {
public:

	void SetupBuffers();

	void Reset();

	void GenDrawCommands(int RenderDistance, int verticalRenderDistance);

	bool AddChunkVertices(const std::vector<uint32_t>& Data, const ChunkPos& pos);

	void DeleteChunkVertices(const ChunkPos& ID);

	void SetMaxSize(size_t size);

	void Draw();

	void Bind();

	void Unbind();

	void Cleanup();

	void Defrager(int iterations);

	void UpdateCommandBufferSize();

	void ErrorCheck();

	Camera* camera; // TODO: Rename

	std::map<size_t, ChunkMemoryPoolOffset> render_list_; //f: Offset -> RenderInfo

	double debug_time_ = 0.0;

	ChunkGPUMemoryPool memory_pool_;
private:
	ChunkDrawCommandBuffer command_buffer_;
	CFrustum frustum_;
	Buffer ibo_, ssbo_;
	VertexArray array_;
	size_t max_buffer_size_ = NULL;

	bool update_commands_ = false;

	FastHashMap<ChunkPos, size_t> render_list_offset_lookup_; //f: ChunkPos  -> Offset
	std::vector<GLint> chunk_shader_pos_;

	std::vector<DrawCommandIndirect> draw_commands_;

	size_t amount_of_chunks_ = 0;
	size_t amount_of_chunks_being_rendered_ = 0;

	Buffer transfer_buffer_;
};