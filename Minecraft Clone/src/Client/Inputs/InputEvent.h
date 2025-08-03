#pragma once

#include <glm/vec2.hpp>
#include <variant>

struct InputEvent {};

struct MouseMove : InputEvent {
  glm::vec2 new_pos_;
  glm::vec2 displacemnent_;
};

struct MouseButtonHold : InputEvent {};

struct MouseButtonRelease : InputEvent {};

struct MouseButtonPress : InputEvent {};

struct MouseScroll : InputEvent {};

struct KeyHold : InputEvent {};

struct KeyPress : InputEvent {};

struct KeyRelease : InputEvent {};

using InputEventType = std::variant<MouseMove, MouseButtonHold,
                                    MouseButtonPress, MouseButtonRelease,
                                    KeyHold, KeyPress, KeyRelease, MouseScroll>;
