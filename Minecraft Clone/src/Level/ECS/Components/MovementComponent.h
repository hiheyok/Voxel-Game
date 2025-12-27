#pragma once

#include <glm/vec3.hpp>

class MovementComponent {
 public:
  MovementComponent();

  void SetTargetPositon(glm::vec3 target);
  void SetMovementSpeed(double speed);
  void SetMovementFlag(bool is_moving);

  double GetMovementSpeed() const;
  glm::vec3 GetTargetPosition() const;
  bool IsMoving() const;
 private:
  // Entity movement speed
  double move_speed_;
  // Flag to check if the entity is moving
  bool is_moving_;
  // The target position of an entity
  glm::vec3 target_position_;
  // Movement type (swimming, walking, sprinting, etc)
};