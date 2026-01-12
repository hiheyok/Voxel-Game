// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Stats/DebugStatsCollector.h"

#include "Client/ClientLevel/ClientCache.h"
#include "Client/ClientLevel/ClientLevel.h"
#include "Client/Player/MainPlayer.h"
#include "Client/Render/ClientRenderer.h"
#include "Client/Render/WorldRender.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Stats/ServerStats.h"
#include "Level/Entity/Mobs/Player.h"
#include "RenderEngine/ChunkRender/TerrainRenderer.h"

DebugStatsCollector::DebugStatsCollector(ServerInterface& interface,
                                         MainPlayer& player,
                                         ClientRenderer& renderer,
                                         ClientLevel& level)
    : interface_{interface},
      player_{player},
      renderer_{renderer},
      level_{level} {}

DebugStats DebugStatsCollector::Collect(double frametime) {
  DebugStats stats;

  // FPS
  stats.fps = static_cast<float>(1.0 / frametime);

  // Player stats
  auto player_props = player_.GetEntityProperties();
  stats.player_pos = player_props.position_;
  stats.player_vel = player_props.velocity_;

  // Terrain render stats
  WorldRender& terrain = renderer_.GetTerrainRender();
  stats.vram_usage_mb =
      static_cast<float>(terrain.renderer_->GetVRAMUsageFull()) / 1000000.0f;
  stats.vram_frag_rate_pct = terrain.renderer_->GetFragmentationRate() * 100.0f;

  if (terrain.build_time_ > 0) {
    stats.mesh_ms = terrain.build_time_ / 1000.0f;
    stats.mesh_s0_pct = terrain.build_stage_0_ / terrain.build_time_ * 100.0f;
    stats.mesh_s1_pct = terrain.build_stage_1_ / terrain.build_time_ * 100.0f;
    stats.mesh_avg_us = terrain.build_time_ / terrain.amount_of_mesh_generated_;
  }
  stats.mesh_queue_size = terrain.GetQueuedSize();

  // Light levels at player position
  BlockPos block_pos{static_cast<int>(stats.player_pos.x),
                     static_cast<int>(stats.player_pos.y),
                     static_cast<int>(stats.player_pos.z)};
  ChunkPos chunk_pos = block_pos.ToChunkPos();
  BlockPos rel_block_pos = block_pos.GetLocalPos();

  if (level_.cache_.CheckChunk(chunk_pos)) {
    Chunk& chunk = level_.cache_.GetChunk(chunk_pos);
    stats.sky_light = chunk.sky_light_->GetLighting(rel_block_pos);
    stats.block_light = chunk.block_light_->GetLighting(rel_block_pos);
  }

  // Server stats
  ServerStats server_stats = interface_.GetServerStats();
  stats.mspt = server_stats.mspt_;
  stats.light_queue_size = server_stats.light_stats_.queue_size_;
  stats.light_update_time =
      server_stats.light_stats_.average_light_update_time_;
  stats.light_duplicates_skipped = server_stats.light_stats_.duplicates_skipped_;

  return stats;
}
