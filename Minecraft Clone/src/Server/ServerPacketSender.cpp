// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Server/ServerPacketSender.h"

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

#include "Core/Interfaces/ClientInterface.h"
#include "Core/Networking/BlockUpdate.h"
#include "Core/Networking/ChunkUpdate.h"
#include "Core/Networking/ECSUpdate.h"
#include "Core/Networking/EntityUpdate.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Chunk/ChunkData.h"
#include "Level/Dimension/Dimension.h"
#include "Level/ECS/ComponentMap.h"
#include "Level/ECS/Components/TransformComponent.h"
#include "Level/ECS/EntityRegistry.h"
#include "Level/ECS/ServerECSManager.h"
#include "Level/ECS/ServerEntitySystems.h"
#include "Level/ECS/Systems/ITransformSystem.h"
#include "Level/Level.h"
#include "Level/World/WorldUpdater.h"

using std::array;
using std::vector;

ServerPacketSender::ServerPacketSender(Level& level, const double& mspt)
    : level_{level}, mspt_{mspt} {}

void ServerPacketSender::SendPackets(ClientInterface* client) {
  if (client == nullptr) return;
  SendEntityUpdatePacket(client);
  SendChunkUpdatePacket(client);
  SendBlockUpdatePacket(client);
  SendECSUpdatePacket(client);
  SendServerStats(client);
  client->SendTimeLastTick();
}

void ServerPacketSender::SendEntityUpdatePacket(ClientInterface* client) {
  std::vector<EntityProperty> spawned_entities =
      level_.main_world_->world_updater_->GetSpawnedEntities();
  std::vector<EntityProperty> entity_updated =
      level_.main_world_->world_updater_->GetUpdatedEntities();
  std::vector<EntityUUID> entity_despawned =
      level_.main_world_->world_updater_->GetRemovedEntities();

  // Work on spawn entities
  for (const auto& entity : spawned_entities) {
    EntityUpdatePacket::EntitySpawn packet;
    packet.properties_ = entity;
    client->SendEntityUpdate(packet);
  }

  // Work on updated entities
  for (const auto& entity : entity_updated) {
    EntityUpdatePacket::EntityMove packet;
    packet.properties_ = entity;
    client->SendEntityUpdate(packet);
  }

  // Work on removed entities
  for (const auto& entity : entity_despawned) {
    EntityUpdatePacket::EntityDespawn packet;
    packet.uuid_ = entity;
    client->SendEntityUpdate(packet);
  }
}

void ServerPacketSender::SendChunkUpdatePacket(ClientInterface* client) {
  const std::vector<ChunkPos>& created_chunks =
      level_.main_world_->world_updater_->GetCreatedChunkPos();
  std::vector<ChunkPos> updated_lights =
      level_.main_world_->world_updater_->GetLightUpdate();

  for (const auto& pos : created_chunks) {
    ChunkContainer& chunk = level_.main_world_->world_->GetChunk(pos);
    ChunkRawData data = chunk.GetRawData();

    ChunkUpdatePacket::AddChunk packet = {.chunk_ = data};
    client->SendChunkUpdates(packet);
  }

  for (const auto& pos : updated_lights) {
    ChunkContainer& chunk = level_.main_world_->world_->GetChunk(pos);
    LightStorage sky_light = *chunk.sky_light_.get();
    LightStorage block_light = *chunk.block_light_.get();

    ChunkUpdatePacket::LightUpdate packet;
    packet.sky_light_ = std::move(sky_light);
    packet.block_light_ = std::move(block_light);
    client->SendChunkUpdates(packet);
  }
}

void ServerPacketSender::SendBlockUpdatePacket(ClientInterface* client) {
  const FastHashMap<ChunkPos, std::vector<BlockPos>>& changed_blocks_ =
      level_.main_world_->world_updater_->GetChangedBlocks();

  for (const auto& [chunk_pos, updates] : changed_blocks_) {
    size_t num_updates = updates.size();

    BlockUpdatePacket::BlockMultiUpdate packet;
    packet.chunk_pos_ = chunk_pos;
    packet.updates_.resize(num_updates);

    std::array<BlockID, kChunkSize3D> block_data_ =
        level_.main_world_->world_->GetChunk(chunk_pos)
            .GetPalette()
            .UnpackAll();

    for (int i = 0; i < num_updates; ++i) {
      packet.updates_[i].first = updates[i];
      packet.updates_[i].second = block_data_[updates[i].GetIndex()];
    }

    client->SendBlockUpdate(std::move(packet));
  }
}

void ServerPacketSender::SendServerStats(ClientInterface* client) {
  ServerStats stats;
  stats.chunk_count_ = 0;  // level_->level_loader_->GetChunkCount();
                           // TODO(hiheyok): Add chunk counter
  stats.mspt_ = mspt_;
  stats.event_queued_ = 0;
  stats.light_stats_ = level_.main_world_->GetLightEngineStats();
  client->SendServerStats(stats);
}

void ServerPacketSender::SendECSUpdatePacket(ClientInterface* client) {
  // Cast to ServerECSManager to access server-specific methods
  auto& server_ecs = static_cast<ServerECSManager&>(
      level_.main_world_->world_->GetECSManager());
  auto& transform_map =
      server_ecs.GetSystems().GetTransformSystem().GetComponentMap();
  auto& registry = server_ecs.GetRegistry();

  // Send transform batch
  ECSUpdatePacket::TransformBatch transform_batch;
  for (const auto& [uuid, transform] : transform_map) {
    ECSUpdatePacket::TransformData data;
    data.uuid_ = uuid;
    data.type_ = registry.GetEntityType(uuid);
    data.component_ = transform;
    transform_batch.transforms_.push_back(data);
  }

  if (!transform_batch.transforms_.empty()) {
    client->SendECSUpdate(ECSUpdatePacket::ECSUpdate{transform_batch});
  }

  // Send entity removal batch (if any entities were removed this tick)
  const auto& removed_entities = registry.GetRemovedEntities();
  if (!removed_entities.empty()) {
    ECSUpdatePacket::EntityRemovalBatch removal_batch;
    removal_batch.removed_uuids_ = removed_entities;
    client->SendECSUpdate(ECSUpdatePacket::ECSUpdate{removal_batch});
  }
}
