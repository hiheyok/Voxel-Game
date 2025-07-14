#pragma once

#include <vector>

#include "Core/Typenames.h"

class ChunkLightTask;
class WorldInterface;
class GameContext;
class LightEngineCache;
class Chunk;

class LightEngine {
 protected:
  class InternalTask {
   public:
    void SetBlockPos(BlockPos block_pos) noexcept;
    void SetChunkPos(ChunkPos chunk_pos) noexcept;
    void SetDirection(int direction) noexcept;
    void SetLightLevel(int lvl) noexcept;

    BlockPos GetBlockPos() const noexcept;
    ChunkPos GetChunkPos() const noexcept;
    int GetDirection() const noexcept;
    int GetLightLevel() const noexcept;

   private:
    /* Structure
    0-12 Positional Bits
    12-16 Light lvl
    16-19 Direction
    */
    BlockPos block_pos_;
    ChunkPos chunk_pos_;
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

  BlockID GetBlock(BlockPos pos);
  bool CheckChunk(ChunkPos pos);
  Chunk* GetChunk(ChunkPos pos);

  virtual void LightChunk(ChunkPos chunk_pos) = 0;
  virtual void CheckNeighborChunk(ChunkPos center_chunk) = 0;
  virtual void SetLightLvl(BlockPos block_pos, int light_lvl) = 0;
  virtual int GetLightLvl(BlockPos block_pos) = 0;
  virtual void CheckBlock(ChunkPos chunk_pos, BlockPos block_pos) = 0;
  virtual void PropagateChanges(const ChunkLightTask& tasks) = 0;

 protected:
  static constexpr size_t kQueueSizeIncrement = 8196;

  GameContext& game_context_;
  WorldInterface& world_;

  LightEngineCache* light_cache_;

  std::vector<InternalTask> decrease_queue_;
  std::vector<InternalTask> increase_queue_;

  size_t enqueue_decrease_pos_ = 0;
  size_t enqueue_increase_pos_ = 0;

  size_t dequeue_decrease_pos_ = 0;
  size_t dequeue_increase_pos_ = 0;
};
