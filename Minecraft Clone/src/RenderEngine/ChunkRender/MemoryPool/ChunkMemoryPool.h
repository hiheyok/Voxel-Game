// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <map>
#include <memory>
#include <vector>

#include "Core/Typenames.h"

class BufferStorage;
class GameContext;

struct BlockVertexFormat;

struct ChunkMemoryPoolOffset {
  size_t mem_offset_ = 0;
  size_t mem_size_ = 0;
  ChunkPos position_;

  ChunkMemoryPoolOffset();
  ChunkMemoryPoolOffset(size_t offset, size_t size);
  ChunkMemoryPoolOffset(size_t offset, size_t size, ChunkPos position);
};

namespace MemoryManagement {

struct MemoryBlock {
  MemoryBlock() {}
  MemoryBlock(size_t memOffset, size_t memSize)
      : offset_(memOffset), size_(memSize) {}

  size_t offset_ = 0;
  size_t size_ = 0;
};

class BlockManagement {
 public:
  void Add(MemoryBlock block);
  void Delete(size_t offset);
  void Delete(std::map<size_t, MemoryBlock>::const_iterator it);

  std::map<size_t, MemoryBlock>::const_iterator GetIterator(
      size_t offset) const;
  std::map<size_t, MemoryBlock>::const_iterator begin() const;
  std::map<size_t, MemoryBlock>::const_iterator end() const;
  std::map<size_t, MemoryBlock>::const_reverse_iterator rbegin() const;
  std::map<size_t, MemoryBlock>::const_reverse_iterator rend() const;
  std::map<size_t, MemoryBlock>::const_iterator lower_bound(size_t k) const;
  std::map<size_t, MemoryBlock>::const_iterator upper_bound(size_t k) const;

  size_t size() const;
  size_t GetFragmentCount() const;

 private:
  std::map<size_t, MemoryBlock> mem_blocks_;
  FastHashMap<size_t, std::map<size_t, MemoryBlock>::const_iterator>
      mem_blocks_iterators_;
};

class MemoryPoolManager {
 public:
  MemoryPoolManager();
  ~MemoryPoolManager();

  void Initialize(size_t spaceAvailable);
  void AllocateSpace(size_t memOffset, size_t memSize);
  void DeallocateSpace(size_t MemOffset, size_t MemSize);

  size_t FindFreeSpace(size_t MemSize) const;
  size_t GetFreeSpaceFragmentCount() const;
  size_t GetReserveSpaceFragmentCount() const;

  BlockManagement free_memory_blocks_;
  BlockManagement reserved_memory_blocks_;

 private:
  size_t pool_size_ = 0;

  std::multimap<size_t, size_t> sorted_mem_sizes_;  // BlockSize, BlockOffset
  FastHashMap<size_t, std::multimap<size_t, size_t>::iterator>
      sorted_mem_sizes_iterators;  // Offset, iterator
};

struct MemoryPoolStatistics {
  double fragmentation_rate_ = 0.0f;
  size_t memory_usage_ = 0;
  size_t full_memory_usage_ = 0;
};
}  // namespace MemoryManagement

class ChunkGPUMemoryPool {
 public:
  explicit ChunkGPUMemoryPool(GameContext&, size_t memoryPoolSize);
  ~ChunkGPUMemoryPool();

  ChunkGPUMemoryPool(ChunkGPUMemoryPool&&) noexcept;
  ChunkGPUMemoryPool& operator=(ChunkGPUMemoryPool&&) noexcept;

  ChunkGPUMemoryPool(const ChunkGPUMemoryPool&) = delete;
  ChunkGPUMemoryPool& operator=(const ChunkGPUMemoryPool&) = delete;

  void DeleteChunk(ChunkPos pos);
  ChunkMemoryPoolOffset AddChunk(const std::vector<BlockVertexFormat>& vertices,
                                 ChunkPos pos);
  ChunkMemoryPoolOffset GetChunkMemoryPoolOffset(ChunkPos pos) const;
  bool CheckChunk(ChunkPos pos) const;
  ChunkMemoryPoolOffset AddChunkStaggingBuffer(ChunkPos pos,
                                               uint64_t blockOffset,
                                               uint64_t blockSize);
  ChunkMemoryPoolOffset AddChunkMove(ChunkPos pos, uint64_t blockOffset,
                                     uint64_t blockSize);
  void Update();
  size_t GetSize() const noexcept;

  GameContext& context_;
  MemoryManagement::MemoryPoolStatistics statistics_;
  std::unique_ptr<BufferStorage> stagging_buffer_;
  std::unique_ptr<BufferStorage> buffer_;
  MemoryManagement::MemoryPoolManager memory_pool_;
  FastHashMap<ChunkPos, ChunkMemoryPoolOffset> chunk_memory_offsets_;
  FastHashMap<size_t, ChunkPos> memory_chunk_offsets_;

 private:
  size_t memory_pool_size_ = 0;
  static constexpr size_t kStaggingBufferSize = 1048576;
};
