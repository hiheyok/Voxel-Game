// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec2.hpp>

#include "Core/Typenames.h"

enum class InputAction {
  kDrawWireFrame,
  kDrawSolids,
  kPrintProfiler,
  kWalkForwards,
  kWalkBackwards,
  kStrafeLeft,
  kStrafeRight,
  kSprint,
  kJump,
  kMoveUp,
  kMoveDown,
  kAttackDestroy,
  kPickBlock,
  kUseItemPlaceBlock,
  kHotbarSlot1,
  kHotbarSlot2,
  kHotbarSlot3,
  kHotbarSlot4,
  kHotbarSlot5,
  kHotbarSlot6,
  kHotbarSlot7,
  kHotbarSlot8,
  kHotbarSlot9,
  kReload,
  kToggleCollusion,
  kExit
};

struct MouseInputs {
  enum class ButtonState { RELEASE, PRESS, HOLD };
  enum class ScrollState { SCROLL_NONE, SCROLL_DOWN, SCROLL_UP };

  ButtonState left_ = ButtonState::RELEASE;
  ButtonState middle_ = ButtonState::RELEASE;
  ButtonState right_ = ButtonState::RELEASE;

  ScrollState scroll_direction_ = ScrollState::SCROLL_NONE;

  glm::dvec2 displacement_ = glm::dvec2(0.f, 0.f);
  glm::dvec2 position_ = glm::dvec2(0.f, 0.f);
};

class InputManager {
 public:
  void UpdateAllKey();
  void PressIndividualKey(int key);
  void ReleaseIndividualKey(int key);

  bool CheckKey(int key) const;
  bool CheckKeyPress(int key) const;
  bool CheckKeyHold(int key) const;
  bool CheckAction(InputAction action) const;

  MouseInputs mouse_;

  float delta_ = 0.f;

 private:
  enum class KeyStatus { PRESS, HOLD };

  FastHashMap<int, KeyStatus> keys_;
};
