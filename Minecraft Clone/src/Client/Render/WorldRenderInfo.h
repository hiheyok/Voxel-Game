// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

// TODO(hiheyok): use std::atomic for some of the stats
struct WorldRenderInfo {
  double build_time_ = 0.0;

  double build_stage_0_ = 0.0;
  double build_stage_1_ = 0.0;
  double build_stage_2_ = 0.0;

  size_t is_face_visible_calls_ = 0;
  size_t get_face_calls_ = 0;
  size_t set_face_calls_ = 0;
  size_t set_face_calls_clocks_ = 0;
  size_t get_texture_calls_ = 0;
  size_t compare_quad_calls_ = 0;

  size_t amount_of_mesh_generated_ = 1;

  size_t mesh_0 = 0;
  size_t mesh_1 = 0;

  size_t horizontal_render_distance_ = 16;
  size_t vertical_render_distance_ = 16;
};
