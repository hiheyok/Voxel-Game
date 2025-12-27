#include "Level/ECS/Components/BlockPlaceComponent.h"

BlockPlaceComponent::BlockPlaceComponent()
    : block_id_(0),
      should_convert_(false),
      break_on_impact_(false) {}
