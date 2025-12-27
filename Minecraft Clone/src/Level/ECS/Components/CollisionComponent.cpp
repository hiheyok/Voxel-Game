#include "Level/ECS/Components/CollisionComponent.h"

CollisionComponent::CollisionComponent()
    : grounded{false},
      touching_wall_nx{false},
      touching_wall_px{false},
      touching_wall_py{false},
      touching_wall_nz{false},
      touching_wall_pz{false},
      in_water{false},
      standing_block{false} {}
  