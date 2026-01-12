#pragma once

#include <vector>

#include "Core/Typenames.h"

class ChunkLightTask;
class WorldInterface;
class GameContext;
class LightEngineCache;
class Chunk;

struct BlockProperties;

enum class EngineType : uint8_t { kBlockLight = 0, kSkyLight = 1 };

template <EngineType kEngineType>
class LightEngine {
 protected:
  struct InternalTask {
    BlockPos block_pos_;
    bool recheck_light_ : 1 = false;
    uint8_t direction_ : 4 = 0;
    uint8_t light_lvl_ : 4 = 0;
  };

 public:
  LightEngine(GameContext&, WorldInterface&);
  virtual ~LightEngine();

  void SetCache(LightEngineCache* cache);

  void Propagate(const ChunkLightTask& chunk_task);
  void PropagateIncrease();
  void PropagateDecrease();
  void EnqueueIncrease(InternalTask);
  void EnqueueDecrease(InternalTask);
  void EnlargeIncreaseQueue();
  void EnlargeDecreaseQueue();
  InternalTask DequeueIncrease() noexcept;
  InternalTask DequeueDecrease() noexcept;
  bool IsIncreaseEmpty() const noexcept;
  bool IsDecreaseEmpty() const noexcept;
  bool TryDequeueIncrease(InternalTask&) noexcept;
  bool TryDequeueDecrease(InternalTask&) noexcept;
  void ResetIncreaseQueue();
  void ResetDecreaseQueue();

  void SetLightLvl(BlockPos block_pos, int light_lvl);
  int GetLightLvl(BlockPos block_pos);
  BlockID GetBlock(BlockPos pos);
  bool CheckChunk(ChunkPos pos);
  Chunk* GetChunk(ChunkPos pos);

  virtual void LightChunk(ChunkPos chunk_pos) = 0;
  virtual void CheckNeighborChunk(ChunkPos center_chunk) = 0;
  virtual void CheckBlock(BlockPos block_pos) = 0;
  virtual void PropagateChanges(const ChunkLightTask& tasks) = 0;

 protected:
  static constexpr size_t kQueueSizeIncrement = 16392;

  GameContext& context_;
  WorldInterface& world_;
  const std::vector<BlockProperties>& properties_;

  LightEngineCache* light_cache_;

  std::vector<InternalTask> decrease_queue_;
  std::vector<InternalTask> increase_queue_;

  size_t enqueue_decrease_pos_ = 0;
  size_t enqueue_increase_pos_ = 0;

  size_t dequeue_decrease_pos_ = 0;
  size_t dequeue_increase_pos_ = 0;

  EngineType type_;
};
