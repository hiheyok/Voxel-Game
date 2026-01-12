// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "RenderEngine/UI/Screens/Screen.h"

class DebugStatsCollector;
class MainPlayer;

// Aggregates all UI callback factories for the client.
class ClientUICallbacks {
 public:
  ClientUICallbacks(DebugStatsCollector& stats_collector,
                    MainPlayer& main_player, double& frametime_ref);

  Screen::TickCallback GetDebugStatsCallback();
  Screen::TickCallback GetHotbarUpdateCallback();

 private:
  DebugStatsCollector& stats_collector_;
  MainPlayer& main_player_;
  double& frametime_ref_;
};
