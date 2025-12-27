#pragma once

#include "Core/Typenames.h"

class BlockPlaceComponent {
 public:
  BlockPlaceComponent();

  // Block to turn into if converted
  BlockID block_id_;
  // Boolean to check if an entity should convert to a block
  bool should_convert_;
  // If block should turn into item if collided on nonsolid
  bool break_on_impact_;
};
