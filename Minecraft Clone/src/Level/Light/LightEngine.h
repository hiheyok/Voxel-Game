#pragma once

#include <vector>

#include "Core/DataStructure/BucketQueue.h"
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

  // Functor to extract priority (light level) from InternalTask
  struct GetTaskPriority {
    size_t operator()(const InternalTask& task) const noexcept {
      return task.light_lvl_;
    }
  };

 public:
  LightEngine(GameContext&, WorldInterface&);
  virtual ~LightEngine();

  void SetCache(LightEngineCache* cache);

  void Propagate(const ChunkLightTask& chunk_task);
  void PropagateIncrease();
  void PropagateDecrease();

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
  static constexpr int kMaxLightLevel = 15;
  static constexpr int kNumLightLevels = kMaxLightLevel + 1;

  GameContext& context_;
  WorldInterface& world_;
  const std::vector<BlockProperties>& properties_;

  LightEngineCache* light_cache_;

  // Bucket queues for O(1) priority-based light propagation
  BucketQueue<InternalTask, kNumLightLevels, GetTaskPriority> increase_queue_;
  BucketQueue<InternalTask, kNumLightLevels, GetTaskPriority> decrease_queue_;

  EngineType type_;
};
