// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Core/Options/Option.h"
#include "Level/Container/EntityContainer.h"
#include "Level/Dimension/DimensionProperties.h"
#include "Level/Event/EventHandler.h"
#include "Level/Event/EventSystem.h"
#include "Level/TerrainGeneration/ChunkGenerator.h"
#include "Level/TerrainGeneration/Generators/GeneratorType.h"
#include "Level/World/World.h"
#include "Level/World/WorldParameters.h"

class WorldInterface;
class CollisionDetector;
class WorldUpdater;
class ChunkGenerator;
class ThreadedLightEngine;
class GameContext;

struct LightEngineStats;

class Dimension {
 public:
  EventSystem event_manager_;

  Dimension(GameContext&, DimensionProperties properties,
            WorldGeneratorID generatorTypeIn);
  virtual ~Dimension();

  virtual void Tick() = 0;

  WorldGeneratorID GetGeneratorType() const;
  bool CheckTickUsed(EventID id, BlockPos pos) const;
  void TickUsed(EventID id, BlockPos pos);
  void EventTick();
  void Update();
  void ResetState();

  LightEngineStats GetLightEngineStats() const noexcept;

  GameContext& context_;
  WorldInterface* world_;
  std::unique_ptr<WorldUpdater> world_updater_;
  std::unique_ptr<CollisionDetector> collision_detector_;

 private:
  std::unique_ptr<World> main_world_;
  std::unique_ptr<ChunkGenerator> chunk_generator_;
  std::unique_ptr<ThreadedLightEngine> light_engine_;

  WorldGeneratorID generator_type_;
  DimensionProperties properties_;
  WorldParameters world_settings_;

  FastHashMap<int, FastHashSet<BlockPos>> tick_usage_;
};
