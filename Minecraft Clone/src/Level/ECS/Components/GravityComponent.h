#pragma once

class GravityComponent {
 public:
  GravityComponent();

  static constexpr double kDefaultTerminalVelocity = 50.0;
  
  // Gravity multiplier relative to world gravity
  double gravity_multiplier_;
  // Max velocity for the entity
  double terminal_velocity_;
  // Is entity weightless?
  bool is_affected_by_gravity_;
};
