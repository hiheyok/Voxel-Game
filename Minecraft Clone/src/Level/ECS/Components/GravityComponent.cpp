#include "Level/ECS/Components/GravityComponent.h"

GravityComponent::GravityComponent()
    : gravity_multiplier_(1.0),
      terminal_velocity_(kDefaultTerminalVelocity),
      is_affected_by_gravity_(true) {}
