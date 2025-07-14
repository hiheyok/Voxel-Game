// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

struct ServerStats {
  ServerStats() = default;
  ~ServerStats() = default;

  double mspt_;
  size_t chunk_count_;
  size_t event_queued_;
  size_t light_engine_queue_size_;
};
