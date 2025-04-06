#include <iterator>
#include <cmath>

#include "RenderEngine/ChunkRenderer/Batch/ChunkBatch.h"
#include "RenderEngine/OpenGL/Buffers/BufferStorage.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "Utils/Timer/Timer.h"

ChunkDrawBatch::ChunkDrawBatch(size_t maxSize) : 
    max_buffer_size_{ maxSize },
    memory_pool_{ maxSize } {}
ChunkDrawBatch::ChunkDrawBatch(ChunkDrawBatch&&) = default;
ChunkDrawBatch::~ChunkDrawBatch() {
    memory_pool_.buffer_->Delete();
    ibo_.Delete();
    ssbo_.Delete();
    array_.~VertexArray();

    draw_commands_.clear();
    render_list_.clear();
    chunk_shader_pos_.clear();
}

void ChunkDrawBatch::SetupBuffers() {
    ibo_.GenBuffer();
    ssbo_.GenBuffer();
    array_.GenArray();

    ibo_.SetType(GL_DRAW_INDIRECT_BUFFER);
    ssbo_.SetType(GL_SHADER_STORAGE_BUFFER);

    ibo_.SetUsage(GL_STATIC_DRAW);
    ssbo_.SetUsage(GL_DYNAMIC_COPY);

    array_.Bind();

    ibo_.SetMaxSize((size_t)(max_buffer_size_ / 100));
    ibo_.InitializeData();

    memory_pool_.buffer_->Bind();
    array_.EnableAttriPTR(0, 1, GL_FLOAT, GL_FALSE, 2, 0);
    array_.EnableAttriPTR(1, 1, GL_FLOAT, GL_FALSE, 2, 1);
    memory_pool_.buffer_->Unbind();
    array_.Unbind();

    ssbo_.SetMaxSize((size_t)(max_buffer_size_ / 100));
    ssbo_.InitializeData();
}

void ChunkDrawBatch::Reset() {
//    MemoryPool.buffer.ResetBuffer();
    ibo_.ResetBuffer();
    ssbo_.ResetBuffer();
    array_.ResetArray();
    SetupBuffers();
}

void ChunkDrawBatch::GenDrawCommands(int renderDistance, int verticalRenderDistance) {

    Timer time;

    if (render_list_.size() > draw_commands_.size()) {
        UpdateCommandBufferSize();
    }

    if (!update_commands_)
        return;

    frustum_.CalculateFrustum(camera_);

    glm::ivec3 position(floor(camera_->position_.x / 16.f), floor(camera_->position_.y / 16.f), floor(camera_->position_.z / 16.f));
    
    int index = 1;

    for (const auto& data : render_list_arr_) {

        int x = data.position_.x;
        int y = data.position_.y;
        int z = data.position_.z;

        float deltaX = static_cast<float>((x - position.x) * (x - position.x));
        float deltaY = static_cast<float>((y - position.y) * (y - position.y));
        float deltaZ = static_cast<float>((z - position.z) * (z - position.z));


        float dx2 = deltaX / (renderDistance * renderDistance);
        float dy2 = deltaY / (verticalRenderDistance * verticalRenderDistance);
        float dz2 = deltaZ / (renderDistance * renderDistance);

        if (dx2 + dy2 + dz2 < 1.f) {
            if (frustum_.SphereInFrustum((float)(x << 4), (float)(y << 4), (float)(z << 4), 24.3f)) { // << 4 means multiply by 4
                draw_commands_[index - 1].set(static_cast<uint32_t>(data.mem_size_ >> 3), 1, static_cast<uint32_t>(data.mem_offset_ >> 3), index);
                chunk_shader_pos_[(index - 1) * 3 + 0] = x;
                chunk_shader_pos_[(index - 1) * 3 + 1] = y;
                chunk_shader_pos_[(index - 1) * 3 + 2] = z;

                index++;
            }
        }
    }


    index--;

    amount_of_chunks_being_rendered_ = index;

    ssbo_.InsertSubData(0, (amount_of_chunks_being_rendered_ * 3) * sizeof(int), chunk_shader_pos_.data());
    ibo_.InsertSubData(0, amount_of_chunks_being_rendered_ * sizeof(DrawCommandIndirect), draw_commands_.data());

}

void ChunkDrawBatch::UpdateCommandBufferSize() {
    draw_commands_.resize(render_list_.size());
    chunk_shader_pos_.resize(render_list_.size() * 3);
}

bool ChunkDrawBatch::AddChunkVertices(const std::vector<uint32_t>& Data, const ChunkPos& pos) {
    //size_t DataSize = Data.size() * sizeof(uint32_t);

    ChunkMemoryPoolOffset memoryPoolBlockData = memory_pool_.AddChunk(Data, pos);

    if (memoryPoolBlockData.mem_offset_ == ULLONG_MAX) {
        return false;
    }
    
    if (render_list_.contains(memoryPoolBlockData.mem_offset_)) {
        size_t idx = render_list_[memoryPoolBlockData.mem_offset_];
        render_list_arr_[idx] = memoryPoolBlockData;;
    } else {
        render_list_[memoryPoolBlockData.mem_offset_] = render_list_arr_.size();
        render_list_arr_.emplace_back(memoryPoolBlockData);
    }

    update_commands_ = true;
//    CommandBuffer.AddDrawCommand(DrawCommandIndirect(memoryPoolBlockData.mem_size_ >> 3, 1, memoryPoolBlockData.mem_offset_ >> 3, 0),x,y,z);
    return true;
}

void ChunkDrawBatch::DeleteChunkVertices(const ChunkPos& id) {
    if (memory_pool_.CheckChunk(id)) {
        ChunkMemoryPoolOffset ChunkMemOffset = memory_pool_.GetChunkMemoryPoolOffset(id);
        if (ChunkMemOffset.mem_offset_ == std::numeric_limits<size_t>::max()) {
            g_logger.LogError("ChunkDrawBatch::DeleteChunkVertices", "Failed to delete chunk: " + std::to_string(id));
            return;
        }

        size_t idx = render_list_[ChunkMemOffset.mem_offset_];
        render_list_[render_list_arr_.back().mem_offset_] = idx;
        std::swap(render_list_arr_.back(), render_list_arr_[idx]);
        render_list_arr_.pop_back();
        render_list_.erase(ChunkMemOffset.mem_offset_);

        memory_pool_.DeleteChunk(id);
        update_commands_ = true;
        
    }
}

void ChunkDrawBatch::Bind() {
    array_.Bind();
    ibo_.Bind();
    memory_pool_.buffer_->Bind();
    ssbo_.Bind();
    ssbo_.BindBase(2);
}

void ChunkDrawBatch::Unbind() {
    ssbo_.UnbindBase(2);
    ssbo_.Unbind();
    ibo_.Unbind();
    memory_pool_.buffer_->Unbind();
    array_.Unbind();
}

void ChunkDrawBatch::Draw(Shader* shader) {
    Bind();
    shader->Use();
    glMultiDrawArraysIndirect(GL_TRIANGLES, (GLvoid*)0, (GLsizei)amount_of_chunks_being_rendered_, 0);
    Unbind();
}

void ChunkDrawBatch::Defrag(size_t iterations) {
    size_t fragmentCount = memory_pool_.memory_pool_.GetFreeSpaceFragmentCount();
    if (fragmentCount == 1) {
        return;
    }

    iterations = std::min(iterations, fragmentCount - 1);

    for (int i = 0; i < iterations && memory_pool_.memory_pool_.free_memory_blocks_.size() != 1; ++i) {
        MemoryManagement::MemoryBlock freeMemoryBlock = memory_pool_.memory_pool_.free_memory_blocks_.begin()->second;
        size_t freeSpaceOffset = freeMemoryBlock.offset_;

        auto reserve = memory_pool_.memory_pool_.reserved_memory_blocks_.GetIterator(freeMemoryBlock.size_ + freeMemoryBlock.offset_);
        MemoryManagement::MemoryBlock reservedBlock = reserve->second;

        ChunkPos pos = memory_pool_.memory_chunk_offsets_[reservedBlock.offset_];
        DeleteChunkVertices(pos);

        ChunkMemoryPoolOffset memoryPoolBlockData;
        if (abs((int)freeMemoryBlock.offset_ - (int)reservedBlock.offset_) <= (int)reservedBlock.size_) { // If it overlap, use the stagging buffer
            memory_pool_.buffer_->CopyTo(memory_pool_.stagging_buffer_.get(), reservedBlock.offset_, 0, reservedBlock.size_);
            memoryPoolBlockData = memory_pool_.AddChunkStaggingBuffer(pos, freeSpaceOffset, reservedBlock.size_);
        } else {
            memory_pool_.buffer_->MoveData(reservedBlock.offset_, freeMemoryBlock.offset_, reservedBlock.size_);
            memoryPoolBlockData = memory_pool_.AddChunkMove(pos, freeSpaceOffset, reservedBlock.size_);
        }

        render_list_[memoryPoolBlockData.mem_offset_] = render_list_arr_.size();
        render_list_arr_.emplace_back(memoryPoolBlockData);

        update_commands_ = true;
    }
}