#pragma once

#include <cstdint>

struct TickStateComponent {
  enum class State : uint8_t {
    kUnactive,
    kActive,
  };

  TickStateComponent();

  State state_ = State::kUnactive;
};
