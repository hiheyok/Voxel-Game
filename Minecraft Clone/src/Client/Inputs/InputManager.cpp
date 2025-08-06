// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Inputs/InputManager.h"

#include <glfw/glfw3.h>

#include "Client/Inputs/InputCodes.h"

void InputManager::UpdateAllKey() noexcept {
  keyboard_.Refresh();
  mouse_.Refresh();
}

bool InputManager::CheckAction(InputAction action) const noexcept {
  // TODO(hiheyok): Use a json file later to map these
  switch (action) {
    case InputAction::kDrawWireFrame: {
      return keyboard_.CheckKey(KEY_F);
    }
    case InputAction::kDrawSolids: {
      return keyboard_.CheckKey(KEY_G);
    }
    case InputAction::kWalkForwards: {
      return keyboard_.CheckKey(KEY_W);
    }
    case InputAction::kWalkBackwards: {
      return keyboard_.CheckKey(KEY_S);
    }
    case InputAction::kStrafeLeft: {
      return keyboard_.CheckKey(KEY_A);
    }
    case InputAction::kStrafeRight: {
      return keyboard_.CheckKey(KEY_D);
    }
    case InputAction::kSprint: {
      return keyboard_.CheckKey(KEY_LEFT_CONTROL);
    }
    case InputAction::kJump: {
      return keyboard_.CheckKeyPress(KEY_SPACE);
    }
    case InputAction::kMoveUp: {
      return keyboard_.CheckKey(KEY_SPACE);
    }
    case InputAction::kMoveDown: {
      return keyboard_.CheckKey(KEY_LEFT_SHIFT);
    }
    case InputAction::kAttackDestroy: {
      return mouse_.CheckButtonPress(MouseInputs::ButtonType::kLeft);
    }
    case InputAction::kPickBlock: {
      return mouse_.CheckButtonPress(MouseInputs::ButtonType::kMiddle);
    }
    case InputAction::kUseItemPlaceBlock: {
      return mouse_.CheckButtonPress(MouseInputs::ButtonType::kRight);
    }
    case InputAction::kPrintProfiler: {
      return keyboard_.CheckKeyPress(KEY_P);
    }
    case InputAction::kExit: {
      return keyboard_.CheckKey(KEY_ESCAPE);
    }
    case InputAction::kHotbarSlot1: {
      return keyboard_.CheckKeyPress(KEY_1);
    }
    case InputAction::kHotbarSlot2: {
      return keyboard_.CheckKeyPress(KEY_2);
    }
    case InputAction::kHotbarSlot3: {
      return keyboard_.CheckKeyPress(KEY_3);
    }
    case InputAction::kHotbarSlot4: {
      return keyboard_.CheckKeyPress(KEY_4);
    }
    case InputAction::kHotbarSlot5: {
      return keyboard_.CheckKeyPress(KEY_5);
    }
    case InputAction::kHotbarSlot6: {
      return keyboard_.CheckKeyPress(KEY_6);
    }
    case InputAction::kHotbarSlot7: {
      return keyboard_.CheckKeyPress(KEY_7);
    }
    case InputAction::kHotbarSlot8: {
      return keyboard_.CheckKeyPress(KEY_8);
    }
    case InputAction::kHotbarSlot9: {
      return keyboard_.CheckKeyPress(KEY_9);
    }
    case InputAction::kReload: {
      return keyboard_.CheckKeyPress(KEY_R);
    }
    case InputAction::kToggleCollusion: {
      return keyboard_.CheckKeyPress(KEY_C);
    }
  }
  return false;
}

glm::vec2 InputManager::GetMouseDisplacement() const noexcept {
  return mouse_.GetDisplacement();
}

glm::vec2 InputManager::GetMousePos() const noexcept { return mouse_.GetPos(); }

MouseInputs::ScrollState InputManager::GetScrollState() const noexcept {
  return mouse_.GetScrollState();
}

std::vector<InputEvent> InputManager::GetEvents() const {
  std::vector<InputEvent> events;

  if (cursor_enable_) {
    // 1. Check for Mouse Movement
    if (mouse_.GetDisplacement() != glm::vec2(0.0f, 0.0f)) {
      events.emplace_back(MouseMove{mouse_.GetPos(), mouse_.GetDisplacement()});
    }

    // 2. Check for Mouse Button Actions
    for (uint8_t i = 0;
         i <= static_cast<uint8_t>(MouseInputs::ButtonType::kLast); ++i) {
      const auto state = mouse_.button_states_[i];
      if (state == MouseInputs::ButtonState::kPress ||
          state == MouseInputs::ButtonState::kRelease) {
        events.emplace_back(
            MouseButtonAction{static_cast<MouseInputs::ButtonType>(i), state});
      }
    }
  }

  // 3. Check for Mouse Scrolling
  if (mouse_.GetScrollState() != MouseInputs::ScrollState::kNone) {
    events.emplace_back(MouseScroll{mouse_.GetScrollState()});
  }

  // 4. Check for Keyboard Actions
  for (int i = 0; i < KeysInputs::kKeyCodeRange; ++i) {
    const auto state = keyboard_.states_[i];
    // Create events for discrete actions: Press, Release, and Repeat
    if (state == KeysInputs::KeyState::kPress ||
        state == KeysInputs::KeyState::kRelease ||
        state == KeysInputs::KeyState::kRepeat) {
      events.emplace_back(KeyAction{i, state});
    }
  }

  return events;
}

glm::vec2 InputManager::GetScreenRes() const noexcept {
  return current_resolution_;
}

void InputManager::UpdateScroll(float x_offset, float y_offset) noexcept {
  mouse_.UpdateScroll(x_offset, y_offset);
}

void InputManager::UpdateMouse(float x_pos, float y_pos) noexcept {
  mouse_.UpdateMouse(x_pos, y_pos);
}

void InputManager::UpdateButton(int button, int action) noexcept {
  mouse_.UpdateButton(button, action);
}

void InputManager::UpdateKeys(int key, int scancode, int action,
                              int mods) noexcept {
  keyboard_.UpdateKeys(key, scancode, action, mods);
}

void InputManager::UpdateResolution(int x, int y) noexcept {
  current_resolution_.x = x;
  current_resolution_.y = y;
}

void InputManager::SetCursor(bool enable) noexcept { cursor_enable_ = enable; }
