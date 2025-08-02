#pragma once

enum class InputEventType {
  kMouseMove,
  kMouseButtonDown,
  kMouseButtonUp,
  kMouseClick,
  kKeyDown
};

struct InputEvent {};

struct MouseMove : InputEvent {

};

struct MouseButtonDown : InputEvent {

};

struct MouseButtonUp : InputEvent {

};

struct MouseClick : InputEvent {

};

struct kKeyDown : InputEvent {

};
