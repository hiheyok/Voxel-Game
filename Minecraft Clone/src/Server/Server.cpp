// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Server/Server.h"

#include <memory>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ClientInterface.h"
#include "Core/Interfaces/InternalInterface.h"
#include "Level/Chunk/ChunkData.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Entity/Properties/EntityProperties.h"
#include "Level/Level.h"
#include "Level/World/WorldUpdater.h"
#include "Utils/Clock.h"
#include "Utils/Math/Ray/Ray.h"
#include "Utils/Timer/Timer.h"

Server::Server(GameContext& game_context) : game_context_{game_context} {}
Server::~Server() = default;

void Server::StartServer(ServerSettings serverSettings) {
  level_ = std::make_unique<Level>(game_context_);
  level_->Start();
  time_ = std::make_unique<Timer>();
  stop_ = false;
  settings_ = std::make_unique<ServerSettings>(serverSettings);
  main_server_loop_ = std::thread(&Server::Loop, this);
}

void Server::Stop() {
  stop_ = true;
  main_server_loop_.join();
  level_->Stop();
}

void Server::Loop() {
  game_context_.logger_->LogDebug("Server::Loop", "Started main server loop");
  while (!stop_) {
    time_->Set();
    // Process Client -> Server events
    ProcessPacket();

    Tick();

    SendPacket();

    mspt_ = time_->GetTimePassed_ms();
    double time_left = (1.0 / settings_->tick_rate_) - mspt_ / 1000.0;

    if (time_left > 0) {
      timerSleep(time_left);
    }

    mspt_ = time_->GetTimePassed_ms();
  }
  game_context_.logger_->LogDebug("Server::Loop",
                                  "Shutting down main server loop");
}

void Server::Tick() {
  level_->main_world_->Tick();
  level_->main_world_->EventTick();
  level_->updateDimensions();
  level_->main_world_->world_updater_->Load();
}

void Server::SendEvent(const Event& eventIn) {
  level_->main_world_->event_manager_.AddEvent(eventIn);
}

EntityUUID Server::SetInternalConnection(InternalInterface* interface) {
  client_interface_ = static_cast<ClientInterface*>(interface);
  std::unique_ptr<Player> player = std::make_unique<Player>();
  EntityUUID uuid =
      level_->main_world_->world_updater_->SetEntity(std::move(player));
  interface->SetPlayerUUID(uuid);
  return uuid;
}

void Server::ProcessPacket() {
  if (client_interface_ == nullptr) return;
  // Process player actions
  ProcessPlayerPackets(client_interface_);
}

void Server::ProcessPlayerPackets(ClientInterface* receiver) {
  std::vector<Packet::PlayerAction> player_packets;
  receiver->PollClientPlayerAction(player_packets);

  for (const auto& packet : player_packets) {
    switch (packet.type_) {
      case PlayerPacket::GET_ITEM:
        // Send packet to player
        {
          const PlayerPacket::PlayerGetItem& get_item =
              std::get<PlayerPacket::PlayerGetItem>(packet.packet_);
          Entity* e =
              level_->main_world_->world_->GetEntity(receiver->GetPlayerUUID());
          Player* player = static_cast<Player*>(e);
          player->entity_inventory_.SetSlot(get_item.slot_, get_item.item_);

          EntityUpdatePacket::EntityInventoryUpdate inventory_update;
          inventory_update.item_ = get_item.item_;
          inventory_update.slot_ = get_item.slot_;
          inventory_update.uuid = receiver->GetPlayerUUID();
          receiver->SendEntityUpdate(inventory_update);
        }
        break;
      case PlayerPacket::DESTROY_BLOCK: {
        const PlayerPacket::PlayerDestroyBlock& destroyBlock =
            std::get<PlayerPacket::PlayerDestroyBlock>(packet.packet_);
        BlockEvent block_event;
        block_event.block_ = game_context_.blocks_->AIR;
        block_event.id_ = game_context_.event_handler_->BlockPlace;
        block_event.pos_ = destroyBlock.pos_;
        level_->main_world_->event_manager_.AddEvent(block_event);
      } break;
      case PlayerPacket::PLACE_BLOCK: {
        const PlayerPacket::PlayerPlaceBlock& placeBlock =
            std::get<PlayerPacket::PlayerPlaceBlock>(packet.packet_);
        BlockEvent block_event;
        block_event.block_ = placeBlock.block_;
        block_event.id_ = game_context_.event_handler_->BlockPlace;
        block_event.pos_ = placeBlock.pos_;
        level_->main_world_->event_manager_.AddEvent(block_event);
      } break;
      case PlayerPacket::MOVE: {
        const PlayerPacket::PlayerMove& movePlayer =
            std::get<PlayerPacket::PlayerMove>(packet.packet_);
        Entity* e =
            level_->main_world_->world_->GetEntity(receiver->GetPlayerUUID());
        e->properties_.acceleration_ = movePlayer.acc_;
        e->properties_.velocity_ = movePlayer.vel_;
        e->properties_.position_ = movePlayer.pos_;
      } break;
    }
  }
}

void Server::SendPacket() {
  if (client_interface_ == nullptr) return;
  SendEntityUpdatePacket(client_interface_);
  SendChunkUpdatePacket(client_interface_);
  SendServerStats(client_interface_);
  client_interface_->SendTimeLastTick();
}

void Server::SendEntityUpdatePacket(ClientInterface* receiver) {
  std::vector<EntityProperty> spawned_entities =
      level_->main_world_->world_updater_->GetSpawnedEntities();
  std::vector<EntityProperty> entity_updated =
      level_->main_world_->world_updater_->GetUpdatedEntities();
  std::vector<EntityUUID> entity_despawned =
      level_->main_world_->world_updater_->GetRemovedEntities();

  // Work on spawn entities
  for (const auto& entity : spawned_entities) {
    EntityUpdatePacket::EntitySpawn packet;
    packet.properties_ = entity;
    receiver->SendEntityUpdate(packet);
  }

  // Work on updated entities
  for (const auto& entity : entity_updated) {
    EntityUpdatePacket::EntityMove packet;
    packet.properties_ = entity;
    receiver->SendEntityUpdate(packet);
  }

  // Work on removed entities
  for (const auto& entity : entity_despawned) {
    EntityUpdatePacket::EntityDespawn packet;
    packet.uuid_ = entity;
    receiver->SendEntityUpdate(packet);
  }
}

void Server::SendChunkUpdatePacket(ClientInterface* receiver) {
  std::vector<ChunkPos> updated_chunks =
      level_->main_world_->world_updater_->GetUpdatedChunkPos();
  std::vector<ChunkPos> updated_lights =
      level_->main_world_->world_updater_->GetUpdatedLightPos();

  for (const auto& pos : updated_chunks) {
    ChunkContainer* chunk = level_->main_world_->world_->GetChunk(pos);
    if (chunk == nullptr) continue;  // TODO(hiheyok): Fix this later
    ChunkRawData data = chunk->GetRawData();

    ChunkUpdatePacket::AddChunk packet = {.chunk_ = data};
    receiver->SendChunkUpdates(packet);
  }

  for (const auto& pos : updated_lights) {
    ChunkContainer* chunk = level_->main_world_->world_->GetChunk(pos);
    LightStorage data = chunk->GetLightData();

    ChunkUpdatePacket::LightUpdate packet;
    packet.light_ = data;
    receiver->SendChunkUpdates(packet);
  }
}

void Server::SendServerStats(ClientInterface* receiver) {
  ServerStats stats;
  stats.chunk_count_ = 0;  // level_->level_loader_->GetChunkCount();
                           // TODO(hiheyok): Add chunk counter
  stats.mspt_ = mspt_;
  stats.event_queued_ = 0;
  receiver->SendServerStats(stats);
}
