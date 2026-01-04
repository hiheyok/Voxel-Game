#include "Level/ECS/Components/MovementComponent.h"

#include <glm/vec3.hpp>

using glm::vec3;

MovementComponent::MovementComponent()
    : move_speed_{1.0}, is_moving_{false}, target_position_{0.0} {}

void MovementComponent::SetTargetPositon(vec3 target) {
  target_position_ = target;
}

void MovementComponent::SetMovementSpeed(double speed) { move_speed_ = speed; }

void MovementComponent::SetMovementFlag(bool is_moving) {
  is_moving_ = is_moving;
}

double MovementComponent::GetMovementSpeed() const { return move_speed_; }

vec3 MovementComponent::GetTargetPosition() const { return target_position_; }

bool MovementComponent::IsMoving() const { return is_moving_; }