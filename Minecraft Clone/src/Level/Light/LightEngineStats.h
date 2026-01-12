#pragma once

struct LightEngineStats {
  size_t queue_size_;
  double average_light_update_time_;
  size_t duplicates_skipped_;  // Count of duplicate requests that were skipped
};
