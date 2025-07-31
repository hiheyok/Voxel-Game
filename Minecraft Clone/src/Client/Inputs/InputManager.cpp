// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Inputs/InputManager.h"

#include <gl/glew.h>

#include "Client/Inputs/InputCodes.h"

void InputManager::UpdateAllKey() {
  for (const auto& key : keys_) {
    keys_[key.first] = KeyStatus::HOLD;
  }

  mouse_.scroll_direction_ = MouseInputs::ScrollState::SCROLL_NONE;

  mouse_.displacement_ = glm::dvec2(0.0, 0.0);

  if (mouse_.left_ == MouseInputs::ButtonState::PRESS) {
    mouse_.left_ = MouseInputs::ButtonState::HOLD;
  }

  if (mouse_.right_ == MouseInputs::ButtonState::PRESS) {
    mouse_.right_ = MouseInputs::ButtonState::HOLD;
  }

  if (mouse_.middle_ == MouseInputs::ButtonState::PRESS) {
    mouse_.middle_ = MouseInputs::ButtonState::HOLD;
  }
}

void InputManager::PressIndividualKey(int key) {
  if (keys_.count(key)) {
    keys_[key] = KeyStatus::PRESS;
  } else {
    keys_[key] = KeyStatus::PRESS;
  }
}

void InputManager::ReleaseIndividualKey(int key) {
  if (keys_.count(key)) {
    keys_.erase(key);
  }
}

bool InputManager::CheckKey(int key) const {
  if (keys_.count(key)) {
    return true;
  }
  return false;
}

bool InputManager::CheckKeyPress(int key) const {
  if (!CheckKey(key)) {
    return false;
  }
  auto it = keys_.find(key);
  return it->second == KeyStatus::HOLD;
}

bool InputManager::CheckKeyHold(int key) const {
  if (!CheckKey(key)) {
    return false;
  }
  auto it = keys_.find(key);
  return it->second == KeyStatus::HOLD;
}

bool InputManager::CheckAction(InputAction action) const {
  // TODO(hiheyok): Use a json file later to map these
  switch (action) {
    case InputAction::kDrawWireFrame: {
      return CheckKey(KEY_F);
    }
    case InputAction::kDrawSolids: {
      return CheckKey(KEY_G);
    }
    case InputAction::kWalkForwards: {
      return CheckKey(KEY_W);
    }
    case InputAction::kWalkBackwards: {
      return CheckKey(KEY_S);
    }
    case InputAction::kStrafeLeft: {
      return CheckKey(KEY_A);
    }
    case InputAction::kStrafeRight: {
      return CheckKey(KEY_D);
    }
    case InputAction::kSprint: {
      return CheckKey(KEY_LEFT_CONTROL);
    }
    case InputAction::kJump: {
      return CheckKeyPress(KEY_SPACE);
    }
    case InputAction::kMoveUp: {
      return CheckKey(KEY_SPACE);
    }
    case InputAction::kMoveDown: {
      return CheckKey(KEY_LEFT_SHIFT);
    }
    case InputAction::kAttackDestroy: {
      return mouse_.left_ == MouseInputs::ButtonState::PRESS;
    }
    case InputAction::kPickBlock: {
      return mouse_.middle_ == MouseInputs::ButtonState::PRESS;
    }
    case InputAction::kUseItemPlaceBlock: {
      return mouse_.right_ == MouseInputs::ButtonState::PRESS;
    }
    case InputAction::kPrintProfiler: {
      return CheckKeyPress(KEY_P);
    }
    case InputAction::kExit: {
      return CheckKey(KEY_ESCAPE);
    }
    case InputAction::kHotbarSlot1: {
      return CheckKeyPress(KEY_1);
    }
    case InputAction::kHotbarSlot2: {
      return CheckKeyPress(KEY_2);
    }
    case InputAction::kHotbarSlot3: {
      return CheckKeyPress(KEY_3);
    }
    case InputAction::kHotbarSlot4: {
      return CheckKeyPress(KEY_4);
    }
    case InputAction::kHotbarSlot5: {
      return CheckKeyPress(KEY_5);
    }
    case InputAction::kHotbarSlot6: {
      return CheckKeyPress(KEY_6);
    }
    case InputAction::kHotbarSlot7: {
      return CheckKeyPress(KEY_7);
    }
    case InputAction::kHotbarSlot8: {
      return CheckKeyPress(KEY_8);
    }
    case InputAction::kHotbarSlot9: {
      return CheckKeyPress(KEY_9);
    }
    case InputAction::kReload: {
      return CheckKeyPress(KEY_R);
    }
    case InputAction::kToggleCollusion: {
      return CheckKeyPress(KEY_C);
    }
    }
  return false;
}
