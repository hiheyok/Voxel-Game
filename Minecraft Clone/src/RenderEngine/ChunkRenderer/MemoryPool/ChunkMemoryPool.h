#pragma once
#include "../../OpenGL/Buffers/Buffer.h"
#include "../../../Level/Chunk/ChunkPos/ChunkPos.h"
#include "../../../Level/Typenames.h"
#include <vector>
#include <map>

struct ChunkMemoryPoolOffset {
	size_t mem_offset_ = NULL;
	size_t mem_size_ = NULL;
	ChunkPos position_;

	ChunkMemoryPoolOffset() {};
	ChunkMemoryPoolOffset(size_t offset, size_t size) : mem_offset_(offset), mem_size_(size) {};
	ChunkMemoryPoolOffset(size_t offset, size_t size, ChunkPos position) : mem_offset_(offset), mem_size_(size), position_(position) {};
};


namespace MemoryManagement {

	struct MemoryBlock {
		MemoryBlock() {};
		MemoryBlock(size_t memOffset, size_t memSize) : offset_(memOffset), size_(memSize){}

		size_t offset_ = NULL;
		size_t size_ = NULL;
	};

	class BlockManagement {
	public:
		void Add(MemoryBlock block) {
			std::map<size_t, MemoryBlock>::iterator it = mem_blocks_.emplace(block.offset_, block).first;
			mem_blocks_iterators_.emplace(block.offset_, it);
		}

		void Delete(size_t offset) {
			if (!mem_blocks_iterators_.count(offset)) {
				return;
			}
			std::map<size_t, MemoryBlock>::iterator it = mem_blocks_iterators_[offset];
			Delete(it);
		}

		void Delete(std::map<size_t, MemoryBlock>::const_iterator it) {
			size_t offset = it->first;

			mem_blocks_.erase(it);
			mem_blocks_iterators_.erase(offset);
		}

		std::map<size_t, MemoryBlock>::iterator getIterator(size_t offset) {
			return mem_blocks_iterators_.count(offset) ? mem_blocks_iterators_[offset] : end();
		}

		std::map<size_t, MemoryBlock>::iterator begin() {
			return mem_blocks_.begin();
		}

		std::map<size_t, MemoryBlock>::iterator end() {
			return mem_blocks_.end();
		}

		std::map<size_t, MemoryBlock>::const_reverse_iterator rbegin() const  {
			return mem_blocks_.rbegin();
		}

		std::map<size_t, MemoryBlock>::const_reverse_iterator rend() const  {
			return mem_blocks_.rend();
		}

		std::map<size_t, MemoryBlock>::const_iterator lower_bound(size_t k) const { //Get latest element less than k
			auto it = mem_blocks_.lower_bound(k);
			it--;
			return it;
		}

		std::map<size_t, MemoryBlock>::const_iterator upper_bound(size_t k) const { //Get latest element greater than k
			return mem_blocks_.upper_bound(k);
		}

		size_t size() const {
			return mem_blocks_.size();
		}

		int GetFragmentCount() const {
			return mem_blocks_.size();
		}

	private:
		std::map<size_t, MemoryBlock> mem_blocks_;
		FastHashMap<size_t, std::map<size_t, MemoryBlock>::iterator> mem_blocks_iterators_;
	};

	class MemoryPoolManager {
	public:
		void Initialize(size_t spaceAvailable) {
			pool_size_ = spaceAvailable;
			free_memory_blocks_.Add(MemoryBlock(0, pool_size_));
			std::multimap<size_t, size_t>::iterator it = sorted_mem_sizes_.emplace(pool_size_, 0); //use for deletions
			sorted_mem_sizes_iterators[0] = it;
		}

		void AllocateSpace(size_t memOffset, size_t memSize) { //Assumes input are valid

			reserved_memory_blocks_.Add(MemoryBlock{ memOffset, memSize });

			std::map<size_t, MemoryBlock>::iterator left = free_memory_blocks_.getIterator(memOffset);

			MemoryBlock current = left->second;

			sorted_mem_sizes_.erase(sorted_mem_sizes_iterators[left->first]);
			sorted_mem_sizes_iterators.erase(left->first);
			free_memory_blocks_.Delete(left);
			

			if (current.size_ == memSize)
				return;

			size_t newFreeSpaceOffset = memOffset + memSize;
			size_t newFreeSpaceSize = current.size_ - memSize;

			MemoryBlock newFreeBlock(newFreeSpaceOffset, newFreeSpaceSize);
			
			std::multimap<size_t, size_t>::iterator it = sorted_mem_sizes_.emplace(std::pair<size_t, size_t>(newFreeSpaceSize, newFreeSpaceOffset)); //use for deletions
			sorted_mem_sizes_iterators[newFreeSpaceOffset] = it;

			free_memory_blocks_.Add(newFreeBlock);
		}

		void DeallocateSpace(size_t MemOffset, size_t MemSize) {
			std::map<size_t, MemoryBlock>::const_iterator rightBlockFree = free_memory_blocks_.upper_bound(MemOffset);
			std::map<size_t, MemoryBlock>::const_iterator leftBlockFree = free_memory_blocks_.lower_bound(MemOffset);

			bool isFreeBlockRight = true;
			bool isFreeBlockLeft = true;

			if (rightBlockFree == free_memory_blocks_.end()) {
				isFreeBlockRight = false;
			} else {
				if (rightBlockFree->second.offset_ != MemOffset + MemSize) {
					isFreeBlockRight = false;
				}
			}

			if (leftBlockFree == free_memory_blocks_.end()) {
				isFreeBlockLeft = false;
			} else {
				if (leftBlockFree->second.offset_ + leftBlockFree->second.size_ != MemOffset) {
					isFreeBlockLeft = false;
				}
			}

			size_t memoryRight = 0, memoryLeft = 0;

			if (isFreeBlockRight) {
				memoryRight = rightBlockFree->second.offset_ + rightBlockFree->second.size_;
			} else {
				memoryRight = MemOffset + MemSize;
			}

			if (isFreeBlockLeft) {
				memoryLeft = leftBlockFree->second.offset_;
			} else {
				memoryLeft = MemOffset;
			}

			size_t freeMemoryOffset = memoryLeft;
			size_t freeMemorySize = memoryRight - memoryLeft;

			//Clear Free Spaces

			if (isFreeBlockLeft) {
				sorted_mem_sizes_.erase(sorted_mem_sizes_iterators[leftBlockFree->first]);
				sorted_mem_sizes_iterators.erase(leftBlockFree->first);
				free_memory_blocks_.Delete(leftBlockFree);
			}
				
			if (isFreeBlockRight) {
				sorted_mem_sizes_.erase(sorted_mem_sizes_iterators[rightBlockFree->first]);
				sorted_mem_sizes_iterators.erase(rightBlockFree->first);
				free_memory_blocks_.Delete(rightBlockFree);
			}
				

			//Add Free Space
			free_memory_blocks_.Add(MemoryBlock(freeMemoryOffset, freeMemorySize));

			std::multimap<size_t, size_t>::iterator it = sorted_mem_sizes_.emplace(freeMemorySize, freeMemoryOffset); //use for deletions
			sorted_mem_sizes_iterators[freeMemoryOffset] = it;

			//Clear Reserve Space
			reserved_memory_blocks_.Delete(MemOffset);
		}

		size_t FindFreeSpace(size_t MemSize) const {
			auto it1 = sorted_mem_sizes_.lower_bound(MemSize);

			if (it1 != sorted_mem_sizes_.end())
				return it1->second;
			return ULLONG_MAX;
		}

		int GetFreeSpaceFragmentCount() const {
			return free_memory_blocks_.GetFragmentCount();
		}

		int GetReserveSpaceFragmentCount() const {
			return reserved_memory_blocks_.GetFragmentCount();
		}

		BlockManagement free_memory_blocks_;
		BlockManagement reserved_memory_blocks_;

	private:
		size_t pool_size_ = 0;
		
		std::multimap<size_t, size_t> sorted_mem_sizes_; // BlockSize, BlockOffset
		FastHashMap<size_t, std::multimap<size_t, size_t>::iterator> sorted_mem_sizes_iterators; // Offset, iterator
	};

	struct MemoryPoolStatistics {
		double fragmentation_rate_ = 0.0f;
		size_t memory_usage_ = 0;
		size_t full_memory_usage_ = 0;
	};
}

class ChunkGPUMemoryPool {
public:

	void Allocate(int MemoryPoolSize) {
		memory_pool_size_ = MemoryPoolSize;

		buffer_.Create(GL_ARRAY_BUFFER, MemoryPoolSize);

		stagging_buffer_.GenBuffer();
		stagging_buffer_.SetType(GL_COPY_WRITE_BUFFER);
		stagging_buffer_.SetUsage(GL_DYNAMIC_COPY);
		stagging_buffer_.SetMaxSize(10000000);
		stagging_buffer_.InitializeData();

		memory_pool_.Initialize(memory_pool_size_);
	}

	void DeleteChunk(ChunkPos pos) {
		if (!chunk_memory_offsets_.count(pos)) {
			Logger.LogDebug("GPU Memory Pool","Attempted to delete non-existant chunk with ID " + std::to_string(pos));
			return;
		}

		ChunkMemoryPoolOffset chunkOffsetData = chunk_memory_offsets_[pos];
		memory_pool_.DeallocateSpace(chunkOffsetData.mem_offset_, chunkOffsetData.mem_size_);
		chunk_memory_offsets_.erase(pos);

		memory_chunk_offsets_.erase(chunkOffsetData.mem_offset_);

		statistics_.memory_usage_ -= chunkOffsetData.mem_size_;
		Update();
	}

	ChunkMemoryPoolOffset AddChunk(const std::vector<uint32_t>& vertices, ChunkPos pos, int side) { //assumes vertices.size() != 0
		size_t blockSize = vertices.size() * sizeof(uint32_t);
		size_t blockOffset = memory_pool_.FindFreeSpace(blockSize);

		if (blockOffset == ULLONG_MAX) { //Check if it is out of space
			Logger.LogError("GPU Memory Pool", "Out of space!");
			return ChunkMemoryPoolOffset{ ULLONG_MAX, ULLONG_MAX };
		}

		ChunkMemoryPoolOffset chunkMemoryBlock;
		chunkMemoryBlock.mem_offset_ = blockOffset;
		chunkMemoryBlock.mem_size_ = blockSize;
		chunkMemoryBlock.position_ = pos;

		memory_pool_.AllocateSpace(blockOffset, blockSize);

		//Send Vertices Data into GPU
		buffer_.InsertData(blockOffset, blockSize, vertices.data());

		//Store Memory Offset
		chunk_memory_offsets_[pos] = chunkMemoryBlock;
		memory_chunk_offsets_[blockOffset] = pos;

		statistics_.memory_usage_ += blockSize;
		Update();
		return chunkMemoryBlock;
	}

	ChunkMemoryPoolOffset GetChunkMemoryPoolOffset(ChunkPos pos) {
		if (chunk_memory_offsets_.count(pos)) {
			return chunk_memory_offsets_[pos];
		}	
		return ChunkMemoryPoolOffset(ULLONG_MAX, ULLONG_MAX, pos);
	}

	bool CheckChunk(ChunkPos pos) const {
		return chunk_memory_offsets_.count(pos);
	}

	ChunkMemoryPoolOffset AddChunkStaggingBuffer(ChunkPos pos, int side, uint64_t blockOffset, uint64_t blockSize) { //assumes vertices.size() != 0
		ChunkMemoryPoolOffset chunkMemoryBlock;
		chunkMemoryBlock.mem_offset_ = blockOffset;
		chunkMemoryBlock.mem_size_ = blockSize;
		chunkMemoryBlock.position_ = pos;

		memory_pool_.AllocateSpace(blockOffset, blockSize);

		buffer_.CopyFrom(stagging_buffer_, 0, blockOffset, blockSize);

		chunk_memory_offsets_[pos] = chunkMemoryBlock;
		memory_chunk_offsets_[blockOffset] = pos;

		statistics_.memory_usage_ += blockSize;
		Update();
		return chunkMemoryBlock;
	}

	void Update() {
		if (memory_pool_.reserved_memory_blocks_.size() == 0) {
			return;
		}

		statistics_.full_memory_usage_ = memory_pool_.reserved_memory_blocks_.rbegin()->first + memory_pool_.reserved_memory_blocks_.rbegin()->second.size_;
		if (statistics_.full_memory_usage_ != 0) {
			statistics_.fragmentation_rate_ = (double)statistics_.memory_usage_ / (double)statistics_.full_memory_usage_;
		}
	}

	MemoryManagement::MemoryPoolStatistics statistics_;
	Buffer stagging_buffer_;
	BufferStorage buffer_;

	MemoryManagement::MemoryPoolManager memory_pool_;
	FastHashMap<ChunkPos, ChunkMemoryPoolOffset> chunk_memory_offsets_;
	FastHashMap<size_t, ChunkPos> memory_chunk_offsets_;

	int memory_pool_size_ = 0;

};