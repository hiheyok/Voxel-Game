#include "ChunkBatch.h"

#include <iterator>
#include "../../../Level/Timer/Timer.h"
using namespace glm;
using namespace std;

void ChunkDrawBatch::SetupBuffers() {
	memory_pool_.Allocate(max_buffer_size_);

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

	memory_pool_.buffer_.Bind();
	array_.EnableAttriPTR(0, 1, GL_FLOAT, GL_FALSE, 2, 0);
	array_.EnableAttriPTR(1, 1, GL_FLOAT, GL_FALSE, 2, 1);
	memory_pool_.buffer_.Unbind();
	array_.Unbind();

	ssbo_.SetMaxSize((size_t)(max_buffer_size_ / 100));
	ssbo_.InitializeData();
}

void ChunkDrawBatch::Reset() {
//	MemoryPool.buffer.ResetBuffer();
	ibo_.ResetBuffer();
	ssbo_.ResetBuffer();
	array_.ResetArray();
	SetupBuffers();
}

void ChunkDrawBatch::Cleanup() {
	memory_pool_.buffer_.Delete();
	ibo_.Delete();
	ssbo_.Delete();
	array_.~VertexArray();

	draw_commands_.clear();
	render_list_.clear();
	chunk_shader_pos_.clear();
	render_list_offset_lookup_.clear();

}

void ChunkDrawBatch::GenDrawCommands(int renderDistance, int verticalRenderDistance) {

	Timer time;

	if (render_list_.size() > draw_commands_.size()) {
		UpdateCommandBufferSize();
	}

	if (!update_commands_)
		return;

	frustum_.CalculateFrustum(camera);

	ivec3 position(floor(camera->position_.x / 16.f), floor(camera->position_.y / 16.f), floor(camera->position_.z / 16.f));
	
	bool useNew = false;

	if (useNew) {
		std::vector<DrawCommandIndirect> cmds = {};
		std::vector<int> cmdPos = {};

		amount_of_chunks_being_rendered_ = command_buffer_.GetDrawCommandsSorted(cmds, cmdPos);

		ssbo_.InsertSubData(0, (amount_of_chunks_being_rendered_ * 3) * sizeof(int), cmdPos.data());
		ibo_.InsertSubData(0, amount_of_chunks_being_rendered_ * sizeof(DrawCommandIndirect), cmds.data());
	}
	else {
		int index = 1;

		for (auto& data_ : render_list_) {
			auto& data = data_.second;

			int x = data.position_.x;
			int y = data.position_.y;
			int z = data.position_.z;
			
			float deltaX = static_cast<float>(x - position.x);
			float deltaY = static_cast<float>(y - position.y);
			float deltaZ = static_cast<float>(z - position.z);


			float dx2 = deltaX * deltaX / (renderDistance * renderDistance);
			float dy2 = deltaY * deltaY / (verticalRenderDistance * verticalRenderDistance);
			float dz2 = deltaZ * deltaZ / (renderDistance * renderDistance);

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

}

void ChunkDrawBatch::UpdateCommandBufferSize() {
	draw_commands_.resize(render_list_.size());
	chunk_shader_pos_.resize(render_list_.size() * 3);
}

bool ChunkDrawBatch::AddChunkVertices(const std::vector<uint32_t>& Data, const ChunkPos& pos) {
	//size_t DataSize = Data.size() * sizeof(uint32_t);

	ChunkMemoryPoolOffset memoryPoolBlockData = memory_pool_.AddChunk(Data, pos, NULL);

	if (memoryPoolBlockData.mem_offset_ == ULLONG_MAX) {
		return false;
	}
	
	render_list_[memoryPoolBlockData.mem_offset_] = memoryPoolBlockData;
	render_list_offset_lookup_[pos] = memoryPoolBlockData.mem_offset_;
	amount_of_chunks_++;
	update_commands_ = true;
//	CommandBuffer.AddDrawCommand(DrawCommandIndirect(memoryPoolBlockData.mem_size_ >> 3, 1, memoryPoolBlockData.mem_offset_ >> 3, 0),x,y,z);
	return true;
}

void ChunkDrawBatch::DeleteChunkVertices(const ChunkPos& id) {
	if (memory_pool_.CheckChunk(id)) {
		ChunkMemoryPoolOffset ChunkMemOffset = memory_pool_.GetChunkMemoryPoolOffset(id);
		if (ChunkMemOffset.mem_offset_ == ULLONG_MAX) {
			Logger.LogError("Chunk Batch", "Failed to delete chunk: " + std::to_string(id));
			return;
		}
		render_list_.erase(ChunkMemOffset.mem_offset_);
		memory_pool_.DeleteChunk(id);
		render_list_offset_lookup_.erase(id);
//		CommandBuffer.DeleteDrawCommand(ChunkPosToPOS(id));
		amount_of_chunks_--;
		update_commands_ = true;
		
	}
}

void ChunkDrawBatch::SetMaxSize(size_t size) {
	max_buffer_size_ = size;
}

void ChunkDrawBatch::Bind() {
	array_.Bind();
	ibo_.Bind();
	memory_pool_.buffer_.Bind();
	ssbo_.Bind();
	ssbo_.BindBase(2);
}

void ChunkDrawBatch::Unbind() {
	ssbo_.UnbindBase(2);
	ssbo_.Unbind();
	ibo_.Unbind();
	memory_pool_.buffer_.Unbind();
	array_.Unbind();
}

void ChunkDrawBatch::Draw() {
	glMultiDrawArraysIndirect(GL_TRIANGLES, (GLvoid*)0, (GLsizei)amount_of_chunks_being_rendered_, 0);
}

void ChunkDrawBatch::Defrager(int cycles) {
	int i = 0;

	int fragmentCount = memory_pool_.memory_pool_.GetFreeSpaceFragmentCount();

	if (fragmentCount == 1) {
		return;
	}

	cycles = std::min(cycles, fragmentCount - 1);

	while (i < cycles) {
		i++;

		if (memory_pool_.memory_pool_.free_memory_blocks_.size() == 1) {
			return;
		}

		MemoryManagement::MemoryBlock freeMemoryBlock = memory_pool_.memory_pool_.free_memory_blocks_.begin()->second;

		int freeSpaceOffset = freeMemoryBlock.offset_;

		std::map<size_t, MemoryManagement::MemoryBlock>::iterator Reserve = memory_pool_.memory_pool_.reserved_memory_blocks_.getIterator(freeMemoryBlock.size_ + freeMemoryBlock.offset_);

		MemoryManagement::MemoryBlock reservedBlock = Reserve->second;

		ChunkPos pos = memory_pool_.memory_chunk_offsets_[reservedBlock.offset_];

		DeleteChunkVertices(pos);

		memory_pool_.buffer_.CopyTo(memory_pool_.stagging_buffer_, reservedBlock.offset_, 0, reservedBlock.size_);
		
		//Add chunk back

		ChunkMemoryPoolOffset memoryPoolBlockData = memory_pool_.AddChunkStaggingBuffer(pos, NULL, freeSpaceOffset, reservedBlock.size_);

		render_list_[memoryPoolBlockData.mem_offset_] = memoryPoolBlockData;
		render_list_offset_lookup_[pos] = memoryPoolBlockData.mem_offset_;
	//	CommandBuffer.AddDrawCommand(DrawCommandIndirect(memoryPoolBlockData.mem_size_ >> 3, 1, memoryPoolBlockData.mem_offset_ >> 3, 0), pos.x, pos.y, pos.z);
		amount_of_chunks_++;
		update_commands_ = true;
	}
}



void ChunkDrawBatch::ErrorCheck() {
	FastHashSet<ChunkPos> usedChunk;
	std::string logs = "";

	for (const auto& c : render_list_) {
		if (usedChunk.count(c.second.position_)) {
			logs += "Chunk: " + std::to_string(c.first) + "\n";
		}
		else {
			usedChunk.insert(c.second.position_);
		}

	}

	Logger.LogDebug("Chunk Batch", "Errors: \n" + logs);


}