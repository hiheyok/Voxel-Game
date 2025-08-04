// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/MemoryPool/ChunkMemoryPool.h"

#include <limits>
#include <map>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/ChunkRender/Mesh/BlockVertexFormat.h"
#include "RenderEngine/OpenGL/Buffers/BufferStorage.h"
#include "Utils/LogUtils.h"

using namespace MemoryManagement;

ChunkMemoryPoolOffset::ChunkMemoryPoolOffset() = default;
ChunkMemoryPoolOffset::ChunkMemoryPoolOffset(size_t offset, size_t size)
    : mem_offset_(offset), mem_size_(size) {}
ChunkMemoryPoolOffset::ChunkMemoryPoolOffset(size_t offset, size_t size,
                                             ChunkPos position)
    : mem_offset_(offset), mem_size_(size), position_(position) {}

void BlockManagement::Add(MemoryBlock block) {
  std::map<size_t, MemoryBlock>::iterator it =
      mem_blocks_.emplace(block.offset_, block).first;
  mem_blocks_iterators_.emplace(block.offset_, it);
}

void BlockManagement::Delete(size_t offset) {
  if (!mem_blocks_iterators_.count(offset)) {
    return;
  }
  std::map<size_t, MemoryBlock>::const_iterator it =
      mem_blocks_iterators_[offset];
  Delete(it);
}

void BlockManagement::Delete(std::map<size_t, MemoryBlock>::const_iterator it) {
  size_t offset = it->first;

  mem_blocks_.erase(it);
  mem_blocks_iterators_.erase(offset);
}

std::map<size_t, MemoryBlock>::const_iterator BlockManagement::GetIterator(
    size_t offset) const {
  if (mem_blocks_iterators_.count(offset)) {
    const auto& it = mem_blocks_iterators_.find(offset);
    return it->second;
  } else {
    return end();
  }
}

std::map<size_t, MemoryBlock>::const_iterator BlockManagement::begin() const {
  return mem_blocks_.begin();
}

std::map<size_t, MemoryBlock>::const_iterator BlockManagement::end() const {
  return mem_blocks_.end();
}

std::map<size_t, MemoryBlock>::const_reverse_iterator BlockManagement::rbegin()
    const {
  return mem_blocks_.rbegin();
}

std::map<size_t, MemoryBlock>::const_reverse_iterator BlockManagement::rend()
    const {
  return mem_blocks_.rend();
}

std::map<size_t, MemoryBlock>::const_iterator BlockManagement::lower_bound(
    size_t k) const {  // Get latest element less than k
  auto it = mem_blocks_.lower_bound(k);
  if (it != mem_blocks_.begin()) {
    it--;
    }
  return it;
}

std::map<size_t, MemoryBlock>::const_iterator BlockManagement::upper_bound(
    size_t k) const {  // Get latest element greater than k
  return mem_blocks_.upper_bound(k);
}

size_t BlockManagement::size() const { return mem_blocks_.size(); }

size_t BlockManagement::GetFragmentCount() const { return mem_blocks_.size(); }

// Other class
MemoryPoolManager::MemoryPoolManager() = default;
MemoryPoolManager::~MemoryPoolManager() = default;

void MemoryPoolManager::Initialize(size_t spaceAvailable) {
  pool_size_ = spaceAvailable;
  free_memory_blocks_.Add(MemoryBlock{0, pool_size_});
  std::multimap<size_t, size_t>::iterator it =
      sorted_mem_sizes_.emplace(pool_size_, 0);  // use for deletions
  sorted_mem_sizes_iterators[0] = it;
}

void MemoryPoolManager::AllocateSpace(
    size_t memOffset, size_t memSize) {  // Assumes input are valid

  reserved_memory_blocks_.Add(MemoryBlock{memOffset, memSize});

  std::map<size_t, MemoryBlock>::const_iterator left =
      free_memory_blocks_.GetIterator(memOffset);

  MemoryBlock current = left->second;

  sorted_mem_sizes_.erase(sorted_mem_sizes_iterators[left->first]);
  sorted_mem_sizes_iterators.erase(left->first);
  free_memory_blocks_.Delete(left);

  if (current.size_ == memSize) return;

  size_t newFreeSpaceOffset = memOffset + memSize;
  size_t newFreeSpaceSize = current.size_ - memSize;

  MemoryBlock newFreeBlock(newFreeSpaceOffset, newFreeSpaceSize);

  std::multimap<size_t, size_t>::iterator it = sorted_mem_sizes_.emplace(
      newFreeSpaceSize, newFreeSpaceOffset);  // use for deletions
  sorted_mem_sizes_iterators[newFreeSpaceOffset] = it;

  free_memory_blocks_.Add(newFreeBlock);
}

void MemoryPoolManager::DeallocateSpace(size_t MemOffset, size_t MemSize) {
  std::map<size_t, MemoryBlock>::const_iterator rightBlockFree =
      free_memory_blocks_.upper_bound(MemOffset);
  std::map<size_t, MemoryBlock>::const_iterator leftBlockFree =
      free_memory_blocks_.lower_bound(MemOffset);

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
    if (leftBlockFree->second.offset_ + leftBlockFree->second.size_ !=
        MemOffset) {
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

  // Clear Free Spaces

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

  // Add Free Space
  free_memory_blocks_.Add(MemoryBlock(freeMemoryOffset, freeMemorySize));

  std::multimap<size_t, size_t>::iterator it = sorted_mem_sizes_.emplace(
      freeMemorySize, freeMemoryOffset);  // use for deletions
  sorted_mem_sizes_iterators[freeMemoryOffset] = it;

  // Clear Reserve Space
  reserved_memory_blocks_.Delete(MemOffset);
}

size_t MemoryPoolManager::FindFreeSpace(size_t MemSize) const {
  auto it1 = sorted_mem_sizes_.lower_bound(MemSize);

  if (it1 != sorted_mem_sizes_.end()) return it1->second;
  return ULLONG_MAX;
}

size_t MemoryPoolManager::GetFreeSpaceFragmentCount() const {
  return free_memory_blocks_.GetFragmentCount();
}

size_t MemoryPoolManager::GetReserveSpaceFragmentCount() const {
  return reserved_memory_blocks_.GetFragmentCount();
}

// Other class
ChunkGPUMemoryPool::ChunkGPUMemoryPool(ChunkGPUMemoryPool&& other) noexcept
    : game_context_{other.game_context_},
      stagging_buffer_{std::move(other.stagging_buffer_)},
      buffer_{std::move(other.buffer_)},
      memory_pool_size_{other.memory_pool_size_},
      chunk_memory_offsets_{std::move(other.chunk_memory_offsets_)},
      memory_chunk_offsets_{std::move(other.memory_chunk_offsets_)},
      statistics_{std::move(other.statistics_)} {
  memory_pool_ = std::move(other.memory_pool_);
}

ChunkGPUMemoryPool& ChunkGPUMemoryPool::operator=(
    ChunkGPUMemoryPool&& other) noexcept {
  if (this != &other) {
    stagging_buffer_ = std::move(other.stagging_buffer_);
    buffer_ = std::move(other.buffer_);
    memory_pool_ = std::move(other.memory_pool_);
    memory_pool_size_ = other.memory_pool_size_;
    chunk_memory_offsets_ = std::move(other.chunk_memory_offsets_);
    memory_chunk_offsets_ = std::move(other.memory_chunk_offsets_);
    statistics_ = std::move(other.statistics_);
  }
  return *this;
}

ChunkGPUMemoryPool::ChunkGPUMemoryPool(GameContext& game_context,
                                       size_t memory_pool_size)
    : game_context_{game_context},
      stagging_buffer_{
          std::make_unique<BufferStorage>(game_context, GL_COPY_WRITE_BUFFER,
                                          kStaggingBufferSize, true, nullptr)},
      buffer_{std::make_unique<BufferStorage>(
          game_context, GL_ARRAY_BUFFER, memory_pool_size, true, nullptr)} {
  memory_pool_size_ = memory_pool_size;
  if (memory_pool_size_ == 0) {
    throw std::logic_error(
        "ChunkGPUMemoryPool::Allocate - Attempted to allocate with zero size.");
  }

  // Use the modified Create method with dynamic=true
  if (!buffer_->IsInitialized()) {
    throw std::logic_error(
        "ChunkGPUMemoryPool::Allocate - Failed to create main buffer storage.");
    memory_pool_size_ = 0;  // Reset size on failure
    return;
  }

  memory_pool_.Initialize(memory_pool_size_);
  game_context_.logger_->LogInfo(
      "ChunkGPUMemoryPool::Allocate",
      "Allocated GPU memory pool. Size: " + std::to_string(memory_pool_size_) +
          " bytes.");
}

ChunkGPUMemoryPool::~ChunkGPUMemoryPool() = default;

void ChunkGPUMemoryPool::DeleteChunk(ChunkPos pos) {
  if (!chunk_memory_offsets_.count(pos)) {
    game_context_.logger_->LogDebug(
        "ChunkGPUMemoryPool::AddChunk",
        "Attempted to delete non-existant chunk with ID " +
            std::to_string(pos));
    return;
  }

  ChunkMemoryPoolOffset chunkOffsetData = chunk_memory_offsets_[pos];
  memory_pool_.DeallocateSpace(chunkOffsetData.mem_offset_,
                               chunkOffsetData.mem_size_);
  chunk_memory_offsets_.erase(pos);

  memory_chunk_offsets_.erase(chunkOffsetData.mem_offset_);

  statistics_.memory_usage_ -= chunkOffsetData.mem_size_;
  Update();
}

ChunkMemoryPoolOffset ChunkGPUMemoryPool::AddChunk(
    const std::vector<BlockVertexFormat>& vertices,
    ChunkPos pos) {  // assumes vertices.size() != 0
  size_t blockSize = vertices.size() * sizeof(vertices[0]);
  size_t blockOffset = memory_pool_.FindFreeSpace(blockSize);

  if (blockOffset ==
      std::numeric_limits<size_t>::max()) {  // Check if it is out of space
    game_context_.logger_->LogError("ChunkGPUMemoryPool::AddChunk",
                                    "Out of space!");
    return ChunkMemoryPoolOffset{std::numeric_limits<size_t>::max(),
                                 std::numeric_limits<size_t>::max()};
  }

  ChunkMemoryPoolOffset chunk_memory_block;
  chunk_memory_block.mem_offset_ = blockOffset;
  chunk_memory_block.mem_size_ = blockSize;
  chunk_memory_block.position_ = pos;

  memory_pool_.AllocateSpace(blockOffset, blockSize);

  // Send Vertices Data into GPU
  buffer_->InsertData(blockOffset, blockSize, vertices.data());

  // Store Memory Offset
  chunk_memory_offsets_[pos] = chunk_memory_block;
  memory_chunk_offsets_[blockOffset] = pos;

  statistics_.memory_usage_ += blockSize;
  Update();
  return chunk_memory_block;
}

ChunkMemoryPoolOffset ChunkGPUMemoryPool::GetChunkMemoryPoolOffset(
    ChunkPos pos) const {
  const auto& it = chunk_memory_offsets_.find(pos);
  if (it != chunk_memory_offsets_.end()) {
    return it->second;
  }
  return ChunkMemoryPoolOffset{std::numeric_limits<size_t>::max(),
                               std::numeric_limits<size_t>::max(), pos};
}

bool ChunkGPUMemoryPool::CheckChunk(ChunkPos pos) const {
  return chunk_memory_offsets_.count(pos);
}

ChunkMemoryPoolOffset ChunkGPUMemoryPool::AddChunkStaggingBuffer(
    ChunkPos pos, uint64_t blockOffset,
    uint64_t blockSize) {  // assumes vertices.size() != 0
  ChunkMemoryPoolOffset chunk_memory_block;
  chunk_memory_block.mem_offset_ = blockOffset;
  chunk_memory_block.mem_size_ = blockSize;
  chunk_memory_block.position_ = pos;

  memory_pool_.AllocateSpace(blockOffset, blockSize);

  buffer_->CopyFrom(stagging_buffer_.get(), 0, blockOffset, blockSize);

  chunk_memory_offsets_[pos] = chunk_memory_block;
  memory_chunk_offsets_[blockOffset] = pos;

  statistics_.memory_usage_ += blockSize;
  Update();
  return chunk_memory_block;
}

ChunkMemoryPoolOffset ChunkGPUMemoryPool::AddChunkMove(
    ChunkPos pos, uint64_t blockOffset,
    uint64_t blockSize) {  // assumes vertices.size() != 0
  ChunkMemoryPoolOffset chunk_memory_block;
  chunk_memory_block.mem_offset_ = blockOffset;
  chunk_memory_block.mem_size_ = blockSize;
  chunk_memory_block.position_ = pos;

  memory_pool_.AllocateSpace(blockOffset, blockSize);

  chunk_memory_offsets_[pos] = chunk_memory_block;
  memory_chunk_offsets_[blockOffset] = pos;

  statistics_.memory_usage_ += blockSize;
  Update();
  return chunk_memory_block;
}

void ChunkGPUMemoryPool::Update() {
  if (memory_pool_.reserved_memory_blocks_.size() == 0) {
    return;
  }

  statistics_.full_memory_usage_ =
      memory_pool_.reserved_memory_blocks_.rbegin()->first +
      memory_pool_.reserved_memory_blocks_.rbegin()->second.size_;

  if (statistics_.full_memory_usage_ != 0) {
    statistics_.fragmentation_rate_ =
        static_cast<double>(statistics_.memory_usage_) /
        statistics_.full_memory_usage_;
  }
}

size_t ChunkGPUMemoryPool::GetSize() const noexcept {
  return memory_pool_size_;
}
