// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientPacketReceiver.h"

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Client/ECS/ClientECSManager.h"
#include "Client/ECS/ClientEntitySystems.h"
#include "Client/Render/WorldRender.h"
#include "ClientLevel/ClientCache.h"
#include "ClientLevel/ClientLevel.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Networking/ChunkUpdate.h"
#include "Core/Networking/ECSUpdate.h"
#include "Core/Networking/Packet.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/ECS/Systems/ITransformSystem.h"
#include "RenderEngine/EntityRender/MultiEntityRender.h"
#include "Utils/LogUtils.h"

using std::get;
using std::make_unique;
using std::move;
using std::unique_ptr;
using std::vector;

ClientPacketReceiver::ClientPacketReceiver(GameContext& context,
                                           ClientLevel& client_level,
                                           WorldRender& terrain_render,
                                           MultiEntityRender& entity_render)
    : context_{context},
      client_level_{client_level},
      terrain_render_{terrain_render},
      entity_render_{entity_render} {}

void ClientPacketReceiver::ProcessPackets(ServerInterface& server) {
  chunks_to_update_.clear();

  ProcessChunkUpdates(server);
  ProcessBlockUpdates(server);
  ProcessEntityUpdates(server);
  ProcessECSUpdates(server);

  // Apply chunk updates to terrain renderer
  if (!chunks_to_update_.empty()) {
    terrain_render_.LoadChunkMultiToRenderer(chunks_to_update_);
  }
}

void ClientPacketReceiver::ProcessChunkUpdates(ServerInterface& server) {
  vector<Packet::ChunkUpdateData> chunk_packets;
  server.PollChunkUpdates(chunk_packets);

  vector<ChunkUpdatePacket::AddChunk> new_chunks;
  vector<ChunkUpdatePacket::LightUpdate> light_updates;

  for (const auto& packet : chunk_packets) {
    switch (packet.type_) {
      case ChunkUpdatePacket::PacketType::ADD_CHUNK: {
        const ChunkUpdatePacket::AddChunk& p =
            get<ChunkUpdatePacket::AddChunk>(packet.packet_);
        new_chunks.push_back(p);
      } break;
      case ChunkUpdatePacket::PacketType::DELETE_CHUNK:
        GAME_ASSERT(false, "Attempted to process unimplemented type");
        break;
      case ChunkUpdatePacket::PacketType::LIGHT_UPDATE: {
        const ChunkUpdatePacket::LightUpdate& p =
            get<ChunkUpdatePacket::LightUpdate>(packet.packet_);
        light_updates.push_back(p);
      } break;
    }
  }

  for (const auto& chunk : new_chunks) {
    const ChunkRawData& data = chunk.chunk_;
    chunks_to_update_.push_back(data.pos_);
    if (client_level_.cache_.CheckChunk(data.pos_)) {
      ChunkContainer& c = client_level_.cache_.GetChunk(data.pos_);
      c.SetData(data);
    } else {
      unique_ptr<Chunk> newChunk = make_unique<Chunk>(context_, data);
      client_level_.cache_.AddChunk(move(newChunk));
    }
  }

  for (const auto& light : light_updates) {
    const LightStorage& sky_light = light.sky_light_;
    const LightStorage& block_light = light.block_light_;

    if (!client_level_.cache_.CheckChunk(sky_light.position_)) {
      continue;
    }

    Chunk& chunk = client_level_.cache_.GetChunk(sky_light.position_);

    *chunk.sky_light_ = sky_light;
    *chunk.block_light_ = block_light;
    chunks_to_update_.push_back(chunk.position_);
  }
}

void ClientPacketReceiver::ProcessBlockUpdates(ServerInterface& server) {
  vector<Packet::BlockUpdate> block_packets;
  server.PollBlockUpdates(block_packets);

  vector<BlockUpdatePacket::BlockMultiUpdate> block_updates;

  for (auto& packet : block_packets) {
    switch (packet.type_) {
      case BlockUpdatePacket::PacketType::kBlockMultiUpdate: {
        BlockUpdatePacket::BlockMultiUpdate& p =
            get<BlockUpdatePacket::BlockMultiUpdate>(packet.packet_);
        block_updates.push_back(move(p));
      } break;
      case BlockUpdatePacket::PacketType::kBlockUpdate:
        GAME_ASSERT(false, "Unhandled packet type");
      default:
        GAME_ASSERT(false, "Error occured");
    }
  }

  for (const auto& block_update : block_updates) {
    ChunkPos chunk_pos = block_update.chunk_pos_;
    ChunkContainer& chunk = client_level_.cache_.GetChunk(chunk_pos);
    bool neighbor_update[6] = {};

    for (auto [block_pos, block] : block_update.updates_) {
      chunk.SetBlockUnsafe(block, block_pos);
      // Check if the block update is next to chunk border
      if (block_pos.x == kChunkDim - 1) neighbor_update[0] = true;
      if (block_pos.x == 0) neighbor_update[1] = true;
      if (block_pos.y == kChunkDim - 1) neighbor_update[2] = true;
      if (block_pos.y == 0) neighbor_update[3] = true;
      if (block_pos.z == kChunkDim - 1) neighbor_update[4] = true;
      if (block_pos.z == 0) neighbor_update[5] = true;
    }

    for (auto direction : Directions<ChunkPos>()) {
      if (!neighbor_update[direction]) {
        continue;
      }

      ChunkPos offset_chunk = chunk_pos + direction;
      // Only queue for rendering if the neighbor chunk exists
      if (client_level_.cache_.CheckChunk(offset_chunk)) {
        chunks_to_update_.push_back(offset_chunk);
      }
    }
    chunks_to_update_.push_back(chunk_pos);
  }
}

void ClientPacketReceiver::ProcessEntityUpdates(ServerInterface& server) {
  vector<EntityProperty> spawnedEntities;
  vector<EntityProperty> updatedEntities;
  vector<EntityUUID> despawnedEntities;

  vector<Packet::EntityUpdate> packets;

  server.PollEntityUpdates(packets);

  for (const auto& packet : packets) {
    switch (packet.type_) {
      case EntityUpdatePacket::PacketType::ENTITY_DESPAWN: {
        const EntityUpdatePacket::EntityDespawn& data =
            get<EntityUpdatePacket::EntityDespawn>(packet.packet_);
        despawnedEntities.push_back(data.uuid_);
        break;
      }
      case EntityUpdatePacket::PacketType::ENTITY_INVENTORY_UPDATE: {
        // N/A
        break;
      }
      case EntityUpdatePacket::PacketType::ENTITY_MOVE: {
        const EntityUpdatePacket::EntityMove& data =
            get<EntityUpdatePacket::EntityMove>(packet.packet_);
        updatedEntities.push_back(data.properties_);
        break;
      }
      case EntityUpdatePacket::PacketType::ENTITY_SPAWN: {
        const EntityUpdatePacket::EntitySpawn& data =
            get<EntityUpdatePacket::EntitySpawn>(packet.packet_);
        spawnedEntities.push_back(data.properties_);
        break;
      }
      default:
        LOG_DEBUG("Default");
    }
  }

  for (const auto& entity : spawnedEntities) {
    client_level_.cache_.InsertEntity(entity);
  }

  for (const auto& entity : updatedEntities) {
    client_level_.cache_.UpdateEntity(entity);
  }

  for (const auto& entity : despawnedEntities) {
    client_level_.cache_.RemoveEntity(entity);
  }

  // Insert it to the renderer now
  for (const auto& entity :
       client_level_.cache_.entities_.GetChangedEntities()) {
    entity_render_.InsertEntity(entity);
  }

  for (const auto& entity :
       client_level_.cache_.entities_.GetRemovedEntitiesUUID()) {
    entity_render_.RemoveEntity(entity);
  }
}

void ClientPacketReceiver::ProcessECSUpdates(ServerInterface& server) {
  vector<ECSUpdatePacket::ECSUpdate> updates;
  server.PollECSUpdates(updates);

  if (updates.empty()) return;

  // Cast to ClientECSManager to access client-specific methods
  auto& client_ecs =
      static_cast<ClientECSManager&>(client_level_.cache_.GetECSManager());
  auto& client_systems = client_ecs.GetConcreteSystems();
  auto& transform_system = client_systems.GetTransformSystem();

  for (const auto& update : updates) {
    switch (update.type_) {
      case ECSUpdatePacket::PacketType::kTransformBatch: {
        const auto& batch =
            get<ECSUpdatePacket::TransformBatch>(update.packet_);

        for (const auto& data : batch.transforms_) {
          // Register entity if not already known
          if (!client_systems.IsValidUUID(data.uuid_)) {
            client_systems.RegisterEntity(data.uuid_, data.type_);
          }

          // Update transform using embedded component
          transform_system.ReplaceComponent(data.uuid_, data.component_);
        }
        break;
      }
      case ECSUpdatePacket::PacketType::kEntityRemoval: {
        const auto& batch =
            get<ECSUpdatePacket::EntityRemovalBatch>(update.packet_);

        for (EntityUUID uuid : batch.removed_uuids_) {
          client_systems.UnregisterEntity(uuid);
        }
        break;
      }
      default:
        GAME_ASSERT(false, "System not handled yet!");
        break;
    }
  }

  // Commit changes to make them visible to renderer
  client_ecs.CommitAll();
}
