// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec3.hpp>

class ServerInterface;
class MainPlayer;
class ClientRenderer;
class ClientLevel;

// Collects debug statistics from various game systems.
struct DebugStats {
  // VRAM stats
  float vram_usage_mb = 0.0f;
  float vram_frag_rate_pct = 0.0f;

  // Player stats
  glm::vec3 player_pos{0.0f};
  glm::vec3 player_vel{0.0f};

  // FPS
  float fps = 0.0f;

  // Mesh stats
  float mesh_ms = 0.0f;
  float mesh_s0_pct = 0.0f;
  float mesh_s1_pct = 0.0f;
  float mesh_avg_us = 0.0f;
  size_t mesh_queue_size = 0;

  // Light stats
  int sky_light = -1;
  int block_light = -1;

  // Server stats
  float mspt = 0.0f;
  size_t light_queue_size = 0;
  float light_update_time = 0.0f;
};

// Gathers all debug statistics into a single struct.
class DebugStatsCollector {
 public:
  DebugStatsCollector(ServerInterface& interface, MainPlayer& player,
                      ClientRenderer& renderer, ClientLevel& level);

  DebugStats Collect(double frametime);

 private:
  ServerInterface& interface_;
  MainPlayer& player_;
  ClientRenderer& renderer_;
  ClientLevel& level_;
};
