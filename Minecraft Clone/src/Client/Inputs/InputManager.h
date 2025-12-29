// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec2.hpp>

#include "Client/Inputs/InputEvent.h"
#include "Client/Inputs/KeysInputs.h"
#include "Client/Inputs/MouseInputs.h"
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
  kToggleCollision,
  kExit
};

class InputManager {
 public:
  void UpdateAllKey() noexcept;
  bool CheckAction(InputAction action) const noexcept;

  // Getters for mouse data
  glm::vec2 GetMouseDisplacement() const noexcept;
  glm::vec2 GetMousePos() const noexcept;

  MouseInputs::ScrollState GetScrollState() const noexcept;
  std::vector<InputEvent> GetEvents() const;
  glm::vec2 GetScreenRes() const noexcept;

  float delta_ = 0.f;

 private:
  friend class Window;

  void UpdateScroll(float x_offset, float y_offset) noexcept;
  void UpdateMouse(float x_pos, float y_pos) noexcept;
  void UpdateButton(int button, int action) noexcept;
  void UpdateKeys(int key, int scancode, int action, int mods) noexcept;
  void UpdateResolution(int x, int y) noexcept;
  void SetCursor(bool enable) noexcept; // Disable mouse event if cursor is disabled

  bool cursor_enable_;

  KeysInputs keyboard_;
  MouseInputs mouse_;
  glm::vec2 current_resolution_;
};
