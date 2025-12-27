#pragma once

#include "Core/Typenames.h"

/**
 * This consist of flags dictating the current state of the entity
 */

struct CollisionComponent {
  CollisionComponent();

  union {
    bool grounded : 1;
    bool touching_wall_ny : 1;
  };
  bool touching_wall_nx : 1;
  bool touching_wall_px : 1;
  bool touching_wall_py : 1;
  bool touching_wall_nz : 1;
  bool touching_wall_pz : 1;
  bool in_water : 1;
  BlockID standing_block;
};
