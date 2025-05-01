#pragma once

struct ServerStats {
  ServerStats() = default;
  ~ServerStats() = default;

  double mspt_;
  size_t chunk_count_;
  size_t event_queued_;
};
