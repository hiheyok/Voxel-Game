#include "Level/Light/ChunkLightTask.h"

#include <cstddef>
#include <utility>
#include <vector>

#include "Core/Position/PositionTypes.h"

ChunkLightTask::ChunkLightTask(ChunkPos chunk_pos,
                               const std::vector<BlockPos>& blocks)
    : chunk_pos_{chunk_pos}, changed_block_{blocks} {}

ChunkLightTask::ChunkLightTask(ChunkPos chunk_pos)
    : chunk_pos_{chunk_pos}, in_queue_{false} {}
ChunkLightTask::~ChunkLightTask() = default;

ChunkLightTask::ChunkLightTask(const ChunkLightTask&) = default;
ChunkLightTask::ChunkLightTask(ChunkLightTask&& other) noexcept
    : changed_block_{std::move(other.changed_block_)},
      chunk_pos_{other.chunk_pos_} {}

ChunkLightTask& ChunkLightTask::operator=(const ChunkLightTask& other) {
  changed_block_ = other.changed_block_;
  return *this;
}
ChunkLightTask& ChunkLightTask::operator=(ChunkLightTask&& other) noexcept {
  changed_block_ = std::move(other.changed_block_);
  return *this;
}

void ChunkLightTask::Insert(BlockPos block_pos) {
  changed_block_.emplace_back(block_pos);
}

void ChunkLightTask::SetInQueue(bool in_queue) noexcept {
  in_queue_ = in_queue;
}

bool ChunkLightTask::CheckInQueue() const noexcept { return in_queue_; }

const std::vector<BlockPos>& ChunkLightTask::GetLightTask() const {
  return changed_block_;
}

ChunkPos ChunkLightTask::GetChunkPos() const noexcept { return chunk_pos_; }

size_t ChunkLightTask::GetTaskCount() const noexcept {
  return changed_block_.size();
}