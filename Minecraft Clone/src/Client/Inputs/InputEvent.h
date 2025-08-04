#pragma once

#include <glm/vec2.hpp>
#include <variant>

#include "Client/Inputs/KeysInputs.h"
#include "Client/Inputs/MouseInputs.h"

struct MouseMove {
  glm::vec2 new_pos_;
  glm::vec2 displacemnent_;
};

struct MouseButtonAction {
  MouseInputs::ButtonType button_type_;
  MouseInputs::ButtonState state_;
};

struct MouseScroll {
  MouseInputs::ScrollState state_;
};

struct KeyAction {
  int key_code_;
  KeysInputs::KeyState state_;
};

using InputEvent =
    std::variant<MouseMove, MouseButtonAction, KeyAction, MouseScroll>;
