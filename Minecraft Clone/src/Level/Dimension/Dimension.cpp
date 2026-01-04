// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Dimension/Dimension.h"

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Dimension/DimensionProperties.h"
#include "Level/ECS/ServerECSManager.h"
#include "Level/Event/Event.h"
#include "Level/Light/ChunkLightTask.h"
#include "Level/Light/LightEngineStats.h"
#include "Level/Light/ThreadedLightEngine.h"
#include "Level/TerrainGeneration/ChunkGenerator.h"
#include "Level/World/WorldUpdater.h"
#include "Utils/Assert.h"

Dimension::Dimension(GameContext& context, DimensionProperties properties,
                     WorldGeneratorID generatorType)
    : context_{context},
      properties_{properties},
      generator_type_{generatorType},
      main_world_{std::make_unique<World>(context)},
      chunk_generator_{std::make_unique<ChunkGenerator>(context, kWorldSeed,
                                                        generatorType)} {
  world_settings_.horizontal_ticking_distance_ =
      context.options_->horizontal_render_distance_;
  world_settings_.vertical_ticking_distance_ =
      context.options_->vertical_render_distance_;

  world_ = static_cast<WorldInterface*>(main_world_.get());
  light_engine_ = std::make_unique<ThreadedLightEngine>(context, *world_);

  world_updater_ = std::make_unique<WorldUpdater>(context_, main_world_.get(),
                                                  world_settings_);

  // Connect ECS to WorldUpdater for block modifications
  auto& server_ecs =
      static_cast<ServerECSManager&>(main_world_->GetECSManager());
  server_ecs.SetWorldUpdater(world_updater_.get());

  collision_detector_ = std::make_unique<CollisionDetector>(
      context_, *main_world_->GetChunkMap());

  if (context_.generators_->GetGenerator(generator_type_)->use_tall_chunks_) {
    world_updater_->tall_generation_ = true;
  }
}

Dimension::~Dimension() = default;

WorldGeneratorID Dimension::GetGeneratorType() const { return generator_type_; }

bool Dimension::CheckTickUsed(EventID id,
                              BlockPos pos) const {  // temp sol
  const auto& it = tick_usage_.find(id);
  if (it == tick_usage_.end()) return false;
  return it->second.contains(pos);
}

void Dimension::TickUsed(EventID id, BlockPos pos) {  // temp solution
  tick_usage_[id].insert(pos);
}

void Dimension::EventTick() {
  std::vector<Event>& event_queue = event_manager_.GetQueue();

  for (size_t i = 0; i < event_queue.size(); i++) {
    const Event& e = event_queue[i];
    switch (e.type_) {
      case NULL_EVENT:
        break;
      case BLOCK_EVENT: {
        const BlockEvent& blockEvent = std::get<BlockEvent>(e.event_data_);
        if (CheckTickUsed(blockEvent.id_, blockEvent.pos_)) {
          continue;
        }
        TickUsed(blockEvent.id_, blockEvent.pos_);
        break;
      }
      case ENTITY_EVENT: {
        const EntityEvent& entityEvent = std::get<EntityEvent>(e.event_data_);
        if ((entityEvent.unique_id_ != 0) &&
            CheckTickUsed(entityEvent.unique_id_, entityEvent.pos_)) {
          continue;
        }
        // TODO(hiheyok): tmp fix rework this later
        if ((entityEvent.unique_id_ != 0)) {
          TickUsed(static_cast<EventID>(entityEvent.entity_uuid_),
                   entityEvent.pos_);
        }
        break;
      }
      default:
        GAME_ASSERT(false, "Event not handled yet!");
    }

    context_.event_handler_->ExecuteEvent(e, this);
  }

  event_queue.clear();
  event_manager_.Swap();

  tick_usage_.clear();

  // Tick all entities

  world_->GetEntityContainer()->Tick(this);
  static_cast<ServerECSManager&>(world_->GetECSManager()).Tick();
}

void Dimension::Update() {
  const std::vector<ChunkPos>& requested_chunks =
      world_updater_->GetRequestedChunks();
  chunk_generator_->Generate(requested_chunks);

  const FastHashMap<ChunkPos, std::vector<BlockPos>>& changed_blocks =
      world_updater_->GetChangedBlocks();
  for (const auto& [chunk_pos, changed_blocks] : changed_blocks) {
    light_engine_->UpdateChunk(ChunkLightTask{chunk_pos, changed_blocks});
  }

  std::vector<std::unique_ptr<Chunk>> chunks = chunk_generator_->GetOutput();
  world_updater_->SetChunk(std::move(chunks));

  const std::vector<ChunkPos>& created_chunks =
      world_updater_->GetCreatedChunkPos();
  for (auto chunk_pos : created_chunks) {
    light_engine_->LightUpChunk(chunk_pos);
  }
}

void Dimension::ResetState() { world_updater_->ResetState(); }

LightEngineStats Dimension::GetLightEngineStats() const noexcept {
  return light_engine_->GetStats();
}
