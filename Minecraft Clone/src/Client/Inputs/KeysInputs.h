#pragma once

#include <array>
#include <cstdint>

class KeysInputs {
 public:
  enum class KeyState : uint8_t { kRelease, kPress, kHold, kRepeat, kNone };
  KeysInputs();
  ~KeysInputs();

  // Setters
  void PressKey(int key_code) noexcept;
  void RepeatKey(int key_code) noexcept;
  void ReleaseKey(int key_code) noexcept;
  void UpdateKeys(int key, int scancode, int action, int mods) noexcept;

  // Runs for every frame
  // If a key already have it's status kRelease, change it to kNone
  // For all keys that have it's status kPress | kRepeat, change it to kHold
  void Refresh() noexcept;

  // Getters
  bool CheckKey(int key_code) const noexcept;
  bool CheckKeyPress(int key_code) const noexcept;
  bool CheckKeyRelease(int key_code) const noexcept;
  bool CheckKeyHold(int key_code) const noexcept;
  bool CheckKeyRepeat(int key_code) const noexcept;

 private:
  // Max key code value is 348
  static constexpr size_t kKeyCodeRange = 348 + 1;

  // Store keys in an array for cache efficiency
  std::array<KeyState, kKeyCodeRange> states_;
};
