#pragma once

struct WorldParameters {
  int horizontal_ticking_distance_ = 16;
  int vertical_ticking_distance_ = 8;

  int spawn_chunk_horizontal_radius_ = 10;
  int spawn_chunk_vertical_radius_ = 10;

  float gravity_ = 10.f;

  int world_tps_ = 20;
};
