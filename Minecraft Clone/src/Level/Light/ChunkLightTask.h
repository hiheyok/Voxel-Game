#pragma once
#include <vector>

#include "Core/Typenames.h"

class ChunkLightTask {
 public:
  ChunkLightTask(ChunkPos, const std::vector<BlockPos>&);
  explicit ChunkLightTask(ChunkPos);
  ~ChunkLightTask();

  ChunkLightTask(const ChunkLightTask&);
  ChunkLightTask(ChunkLightTask&&) noexcept;

  ChunkLightTask& operator=(const ChunkLightTask&);
  ChunkLightTask& operator=(ChunkLightTask&&) noexcept;

  void Insert(BlockPos);
  void SetInQueue(bool) noexcept;
  bool CheckInQueue() const noexcept;

  [[nodiscard]] const std::vector<BlockPos>& GetLightTask() const;
  [[nodiscard]] ChunkPos GetChunkPos() const noexcept;
  [[nodiscard]] size_t GetTaskCount() const noexcept;

 private:
  std::vector<BlockPos> changed_block_;
  const ChunkPos chunk_pos_;
  bool in_queue_;
};
