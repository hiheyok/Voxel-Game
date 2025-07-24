// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientPlay.h"

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Client/IO/IO.h"
#include "Client/Player/MainPlayer.h"
#include "Client/Profiler/PerformanceProfiler.h"
#include "Client/Render/DebugScreen/DebugScreen.h"
#include "Client/Render/WorldRender.h"
#include "ClientLevel/ClientCache.h"
#include "ClientLevel/ClientLevel.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Networking/ChunkUpdate.h"
#include "Core/Networking/Packet.h"
#include "Core/Networking/PlayerAction.h"
#include "Core/Options/Option.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Entity/Entities.h"
#include "Level/Entity/Mobs/Player.h"
#include "RenderEngine/ChunkRender/TerrainRenderer.h"
#include "RenderEngine/EntityRender/MultiEntityRender.h"
#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "RenderEngine/Window.h"
#include "Utils/LogUtils.h"

ClientPlay::ClientPlay(GameContext& game_context, ServerInterface* interface,
                       Window* window, PerformanceProfiler* profiler)
    : game_context_{game_context},
      interface_{interface},
      framebuffer_{std::make_unique<TexturedFrameBuffer>(game_context)},
      client_level_{std::make_unique<ClientLevel>(game_context)},
      main_player_{std::make_unique<MainPlayer>(game_context, window, interface,
                                                &client_level_->cache_)},
      debug_screen_{std::make_unique<DebugScreen>(game_context)},
      entity_render_{std::make_unique<MultiEntityRender>(
          game_context, main_player_->GetPlayerPOV())},
      terrain_render_{std::make_unique<WorldRender>(
          game_context, main_player_->GetPlayerPOV())} {
  main_player_->SetPlayerPosition(0., 50, 0.);
  main_player_->SetPlayerRotation(-135.f, -30.);

  debug_screen_->Initialize(window->GetWindow());
  debug_screen_->SetUpdateRate(200);
  entity_render_->Initialize(profiler);
  entity_render_->SetWindow(window->GetWindow());

  // Send server update on player position
  PlayerPacket::PlayerMove packet;
  packet.acc_ = main_player_->player_->properties_.acceleration_;
  packet.vel_ = main_player_->player_->properties_.velocity_;
  packet.pos_ = main_player_->player_->properties_.position_;
  interface->SendPlayerAction(packet);
  terrain_render_->Start(window->GetWindow(), &client_level_->cache_, profiler);
  framebuffer_->GenBuffer(
      window->GetProperties().window_size_x_,
      window->GetProperties().window_size_y_,
      static_cast<float>(game_context_.options_->graphics_scale_), GL_RGB);
}
ClientPlay::~ClientPlay() = default;

void ClientPlay::Render(Window* window) {
  framebuffer_->BindFBO();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!window->GetProperties().draw_solid_) window->RenderLines();

  // entity_render_->Render();
  terrain_render_->Render();
  entity_render_->Render();

  if (!window->GetProperties().draw_solid_) window->RenderSolid();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  framebuffer_->UnbindFBO();
  framebuffer_->Render();

  main_player_->RenderGUIs();
  debug_screen_->Render();
}

void ClientPlay::Update(Window* window) {
  if (window->GetProperties().window_size_dirty_) {
    window->GetProperties().window_size_dirty_ = false;

    framebuffer_->Clear();
    framebuffer_->GenBuffer(
        window->GetProperties().window_size_x_,
        window->GetProperties().window_size_y_,
        static_cast<float>(game_context_.options_->graphics_scale_));
  }

  if (window->GetUserInputs().CheckKeyPress(GLFW_KEY_R)) {
    entity_render_->Reload();
  }

  main_player_->Update(window->GetUserInputs());

  entity_render_->SetTimePastTick(interface_->time.GetTimePassed_s());
  entity_render_->Update();

  UpdateChunks();
  UpdateEntities();

  // Send player pos update

  PlayerPacket::PlayerMove playerMove;
  playerMove.acc_ = main_player_->player_->properties_.acceleration_;
  playerMove.vel_ = main_player_->player_->properties_.velocity_;
  playerMove.pos_ = main_player_->player_->properties_.position_;

  interface_->SendPlayerAction(playerMove);

  UpdateDebugStats();
}

void ClientPlay::UpdateDebugStats() {
  ServerStats stats = interface_->GetServerStats();
  glm::vec3 player_pos = main_player_->GetEntityProperties().position_;
  BlockPos block_pos{player_pos.x, player_pos.y, player_pos.z};
  BlockPos rel_block_pos = block_pos.GetLocalPos();
  int sky_lvl = -1;
  int block_lvl = -1;
  if (client_level_->cache_.CheckChunk(block_pos.ToChunkPos())) {
    sky_lvl = client_level_->cache_.GetChunk(block_pos.ToChunkPos())
                  ->sky_light_->GetLighting(rel_block_pos);
    block_lvl = client_level_->cache_.GetChunk(block_pos.ToChunkPos())
                    ->block_light_->GetLighting(rel_block_pos);
  }

  debug_screen_->EditText(
      "Stat1",
      "VRAM Usage: " +
          std::to_string(static_cast<double>(
                             terrain_render_->renderer_->GetVRAMUsageFull()) /
                         1000000.0) +
          " MB");
  debug_screen_->EditText("Stat2", "XYZ: " + std::to_string(player_pos.x) +
                                       "/" + std::to_string(player_pos.y) +
                                       "/" + std::to_string(player_pos.z));
  debug_screen_->EditText(
      "Stat3",
      "Velocity XYZ: " +
          std::to_string(main_player_->GetEntityProperties().velocity_.x) +
          "/" +
          std::to_string(main_player_->GetEntityProperties().velocity_.y) +
          "/" +
          std::to_string(main_player_->GetEntityProperties().velocity_.z));
  debug_screen_->EditText(
      "Stat4",
      "VRAM Fragmentation Rate: " +
          std::to_string(terrain_render_->renderer_->GetFragmentationRate() *
                         100) +
          "%");
  debug_screen_->EditText("Stat5", "FPS: " + std::to_string(1.0 / frametime_));

  debug_screen_->EditText(
      "Stat6",
      "Mesh Stats (ms) Total/S0/S1/S2: " +
          std::to_string(terrain_render_->build_time_ / 1000.f) + "/" +
          std::to_string(100.0 * terrain_render_->build_stage_0_ /
                         terrain_render_->build_time_) +
          "/" +
          std::to_string(100.0 * terrain_render_->build_stage_1_ /
                         terrain_render_->build_time_) +
          "/" +
          std::to_string(terrain_render_->build_time_ /
                         terrain_render_->amount_of_mesh_generated_) +
          "/" +
          std::to_string((double)terrain_render_->set_face_calls_clocks_ /
                         terrain_render_->set_face_calls_));

  debug_screen_->EditText("Stat7",
                          "Mesh Engine Queued: " +
                              std::to_string(terrain_render_->GetQueuedSize()));
  debug_screen_->EditText("Stat8", "Light Level: " + std::to_string(sky_lvl) +
                                       " : " + std::to_string(block_lvl));
  debug_screen_->EditText("Stat9",
                          "Server Tick (MSPT): " + std::to_string(stats.mspt_));
  debug_screen_->EditText(
      "Stat10",
      "Light Engine Queue: " + std::to_string(stats.light_stats_.queue_size_) +
          " | Update time: " +
          std::to_string(stats.light_stats_.average_light_update_time_));
  debug_screen_->Update();
}

void ClientPlay::UpdateChunks() {
  std::vector<Packet::ChunkUpdateData> chunk_packets;
  std::vector<Packet::BlockUpdate> block_packets;
  interface_->PollChunkUpdates(chunk_packets);
  interface_->PollBlockUpdates(block_packets);

  std::vector<ChunkUpdatePacket::AddChunk> new_chunks;
  std::vector<ChunkUpdatePacket::LightUpdate> light_updates;
  std::vector<BlockUpdatePacket::BlockMultiUpdate> block_updates;

  for (const auto& packet : chunk_packets) {
    switch (packet.type_) {
      case ChunkUpdatePacket::PacketType::ADD_CHUNK: {
        const ChunkUpdatePacket::AddChunk& p =
            std::get<ChunkUpdatePacket::AddChunk>(packet.packet_);
        new_chunks.push_back(p);
      } break;
      case ChunkUpdatePacket::PacketType::DELETE_CHUNK:
        throw std::runtime_error("ClientPlay::UpdateChunks - Unimplented type");
        break;
      case ChunkUpdatePacket::PacketType::LIGHT_UPDATE: {
        const ChunkUpdatePacket::LightUpdate& p =
            std::get<ChunkUpdatePacket::LightUpdate>(packet.packet_);
        light_updates.push_back(p);
      } break;
    }
  }

  FastHashSet<ChunkPos> chunk_render_update;

  for (auto& packet : block_packets) {
    switch (packet.type_) {
      case BlockUpdatePacket::PacketType::kBlockMultiUpdate: {
        BlockUpdatePacket::BlockMultiUpdate& p =
            std::get<BlockUpdatePacket::BlockMultiUpdate>(packet.packet_);
        block_updates.push_back(std::move(p));
      } break;
      case BlockUpdatePacket::PacketType::kBlockUpdate:
        throw std::runtime_error("Not handled yet!");
      default:
        throw std::runtime_error("Not supposed to be ran!");
    }
  }

  for (const auto& chunk : new_chunks) {
    const ChunkRawData& data = chunk.chunk_;
    chunk_render_update.insert(data.pos_);
    if (client_level_->cache_.CheckChunk(data.pos_)) {
      ChunkContainer* c = client_level_->cache_.GetChunk(data.pos_);
      c->SetData(data);
    } else {
      std::unique_ptr<Chunk> newChunk =
          std::make_unique<Chunk>(game_context_, data);
      client_level_->cache_.AddChunk(std::move(newChunk));
    }
  }

  for (const auto& block_update : block_updates) {
    // Additional stuff to make sure the chunks get updates
    ChunkPos chunk_pos = block_update.chunk_pos_;
    ChunkContainer* chunk = client_level_->cache_.GetChunk(chunk_pos);
    bool neighbor_update[6] = {};

    for (auto [block_pos, block] : block_update.updates_) {
      chunk->SetBlockUnsafe(block, block_pos);
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
      chunk_render_update.insert(offset_chunk);
    }
    chunk_render_update.insert(chunk_pos);
  }

  for (const auto& light : light_updates) {
    const LightStorage& sky_light = light.sky_light_;
    const LightStorage& block_light = light.block_light_;

    if (!client_level_->cache_.CheckChunk(sky_light.position_)) {
      continue;
      game_context_.logger_->LogWarn("ClientPlay::UpdateChunks",
                                     "Chunk doens't exist!");
    }

    Chunk* chunk = client_level_->cache_.GetChunk(sky_light.position_);

    *chunk->sky_light_ = sky_light;
    *chunk->block_light_ = block_light;
    chunk_render_update.insert(chunk->position_);
  }

  std::vector<ChunkPos> to_update(chunk_render_update.begin(),
                                  chunk_render_update.end());
  terrain_render_->Update(to_update);
}

void ClientPlay::UpdateEntities() {
  std::vector<EntityProperty> spawnedEntities;
  std::vector<EntityProperty> updatedEntities;
  std::vector<EntityUUID> despawnedEntities;

  std::vector<Packet::EntityUpdate> packets;

  interface_->PollEntityUpdates(packets);

  for (const auto& packet : packets) {
    switch (packet.type_) {
      case EntityUpdatePacket::PacketType::ENTITY_DESPAWN: {
        const EntityUpdatePacket::EntityDespawn& data =
            std::get<EntityUpdatePacket::EntityDespawn>(packet.packet_);
        despawnedEntities.push_back(data.uuid_);
        break;
      }
      case EntityUpdatePacket::PacketType::ENTITY_INVENTORY_UPDATE: {
        // const EntityUpdatePacket::EntityInventoryUpdate& data =
        //     std::get<EntityUpdatePacket::EntityInventoryUpdate>(packet.packet_);
        // N/A
        break;
      }
      case EntityUpdatePacket::PacketType::ENTITY_MOVE: {
        const EntityUpdatePacket::EntityMove& data =
            std::get<EntityUpdatePacket::EntityMove>(packet.packet_);
        updatedEntities.push_back(data.properties_);
        break;
      }
      case EntityUpdatePacket::PacketType::ENTITY_SPAWN: {
        const EntityUpdatePacket::EntitySpawn& data =
            std::get<EntityUpdatePacket::EntitySpawn>(packet.packet_);
        spawnedEntities.push_back(data.properties_);
        break;
      }
      default:
        game_context_.logger_->LogDebug("ClientPlay::UpdateEntities",
                                        "Default");
    }
  }

  for (const auto& entity : spawnedEntities) {
    client_level_->cache_.InsertEntity(entity);
  }

  for (const auto& entity : updatedEntities) {
    client_level_->cache_.UpdateEntity(entity);
  }

  for (const auto& entity : despawnedEntities) {
    client_level_->cache_.RemoveEntity(entity);
  }

  // Insert it to the renderer now
  for (const auto& entity :
       client_level_->cache_.entities_.GetChangedEntities()) {
    entity_render_->InsertEntity(entity);
  }

  for (const auto& entity :
       client_level_->cache_.entities_.GetRemovedEntitiesUUID()) {
    entity_render_->RemoveEntity(entity);
  }
}
