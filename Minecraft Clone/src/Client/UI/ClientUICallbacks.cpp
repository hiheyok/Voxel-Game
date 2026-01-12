// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/UI/ClientUICallbacks.h"

#include "Client/Player/MainPlayer.h"
#include "Client/Stats/DebugStatsCollector.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Item/ItemStack.h"
#include "RenderEngine/UI/Screens/DebugOverlay.h"
#include "RenderEngine/UI/Screens/PlayerHud.h"

ClientUICallbacks::ClientUICallbacks(DebugStatsCollector& stats_collector,
                                     MainPlayer& main_player,
                                     double& frametime_ref)
    : stats_collector_{stats_collector},
      main_player_{main_player},
      frametime_ref_{frametime_ref} {}

Screen::TickCallback ClientUICallbacks::GetDebugStatsCallback() {
  return [this](Screen* screen) {
    DebugOverlay* debug_screen = static_cast<DebugOverlay*>(screen);
    int duration = debug_screen->timer.GetTimePassed_ms();

    if (duration <= debug_screen->update_rate_) {
      return;
    }

    debug_screen->timer.Set();

    DebugStats stats = stats_collector_.Collect(frametime_ref_);

    debug_screen->Edit(0, "VRAM Usage {} MB", stats.vram_usage_mb);
    debug_screen->Edit(1, "Position: x={}, y={}, z={}", stats.player_pos.x,
                       stats.player_pos.y, stats.player_pos.z);
    debug_screen->Edit(2, "Velocity: x={}, y={}, z={}", stats.player_vel.x,
                       stats.player_vel.y, stats.player_vel.z);
    debug_screen->Edit(3, "VRAM Fragmentation Rate: {}%",
                       stats.vram_frag_rate_pct);
    debug_screen->Edit(4, "FPS: {}", stats.fps);
    debug_screen->Edit(5, "Mesh Stats (ms) Total={} S0={} S1={} Avg={} us",
                       stats.mesh_ms, stats.mesh_s0_pct, stats.mesh_s1_pct,
                       stats.mesh_avg_us);
    debug_screen->Edit(6, "Mesh Engine Queued: {}", stats.mesh_queue_size);
    debug_screen->Edit(7, "Light Level: Sky={}, Block={}", stats.sky_light,
                       stats.block_light);
    debug_screen->Edit(8, "Server Tick (MSPT): {}", stats.mspt);
    debug_screen->Edit(9, "Light Engine Queue: {} | Update time: {}",
                       stats.light_queue_size, stats.light_update_time);
  };
}

Screen::TickCallback ClientUICallbacks::GetHotbarUpdateCallback() {
  return [this](Screen* screen) {
    PlayerHud* player_hud = static_cast<PlayerHud*>(screen);
    auto& inventory = main_player_.player_->entity_inventory_;

    // Update slot contents
    for (int i = 0; i < 9; i++) {
      ItemStack item = inventory.GetItem(i);
      if (item.IsInitialized()) {
        player_hud->UpdateSlot(i, item.item_);
      }
    }

    // Update selection indicator position
    player_hud->UpdateSelectedSlot(inventory.right_hand_slot_);
  };
}
