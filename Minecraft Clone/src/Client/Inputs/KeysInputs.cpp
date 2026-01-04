#include "Client/Inputs/KeysInputs.h"

#include <glfw/glfw3.h>

#include "Utils/Assert.h"

KeysInputs::KeysInputs() { states_.fill(KeyState::kNone); }

KeysInputs::~KeysInputs() = default;

void KeysInputs::PressKey(int key_code) noexcept {
  states_[key_code] = KeyState::kPress;
}

void KeysInputs::RepeatKey(int key_code) noexcept {
  states_[key_code] = KeyState::kRepeat;
}

void KeysInputs::ReleaseKey(int key_code) noexcept {
  states_[key_code] = KeyState::kRelease;
}

void KeysInputs::Refresh() noexcept {
  for (auto& state : states_) {
    if (state == KeyState::kRelease) {
      state = KeyState::kNone;
    }

    if (state == KeyState::kRepeat || state == KeyState::kPress) {
      state = KeyState::kHold;
    }
  }
}

void KeysInputs::UpdateKeys(int key, int scancode, int action,
                            int mods) noexcept {
  switch (action) {
    case GLFW_PRESS: {
      PressKey(key);
      break;
    }
    case GLFW_REPEAT: {
      RepeatKey(key);
      break;
    }
    case GLFW_RELEASE: {
      ReleaseKey(key);
      break;
    }
    default: {
      GAME_ASSERT(false, "Invalid action");
    }
  }
}

bool KeysInputs::CheckKey(int key_code) const noexcept {
  return states_[key_code] != KeyState::kNone;
}

bool KeysInputs::CheckKeyPress(int key_code) const noexcept {
  return states_[key_code] == KeyState::kPress;
}

bool KeysInputs::CheckKeyRelease(int key_code) const noexcept {
  return states_[key_code] == KeyState::kRelease;
}

bool KeysInputs::CheckKeyHold(int key_code) const noexcept {
  return CheckKeyRepeat(key_code) || states_[key_code] == KeyState::kHold;
}

bool KeysInputs::CheckKeyRepeat(int key_code) const noexcept {
  return states_[key_code] == KeyState::kRepeat;
}
