#pragma once
#include <list>
#include <map>
#include <array>
#include <gl/glew.h>
#include <GLFW/glfw3.h>


#include "Level/Chunk/Chunk.h"
#include "RenderEngine/Camera/camera.h"
#include "RenderEngine/ChunkRenderer/Batch/RenderCommandBuffer.h"
#include "RenderEngine/Frustum/frustum.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/ChunkRenderer/MemoryPool/ChunkMemoryPool.h"
#include "Utils/LogUtils.h"
#include "Utils/MathHelper.h"

class ChunkDrawBatch {
public:
    ChunkDrawBatch() = default;
    ChunkDrawBatch(const ChunkDrawBatch&) = delete;
    ChunkDrawBatch(ChunkDrawBatch&&) = default;

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

    void Defrager(size_t iterations);

    void UpdateCommandBufferSize();

    void ErrorCheck();

    Camera* camera = nullptr; // TODO: Rename

    std::map<size_t, ChunkMemoryPoolOffset> render_list_; //f: Offset -> RenderInfo

    double debug_time_ = 0.0;

    ChunkGPUMemoryPool memory_pool_;
private:
    ChunkDrawCommandBuffer command_buffer_;
    CFrustum frustum_;
    Buffer ibo_, ssbo_;
    VertexArray array_;
    size_t max_buffer_size_ = 0;

    bool update_commands_ = false;

    FastHashMap<ChunkPos, size_t> render_list_offset_lookup_; //f: ChunkPos  -> Offset
    std::vector<GLint> chunk_shader_pos_;

    std::vector<DrawCommandIndirect> draw_commands_;

    size_t amount_of_chunks_ = 0;
    size_t amount_of_chunks_being_rendered_ = 0;

    Buffer transfer_buffer_;
};