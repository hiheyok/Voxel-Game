#pragma once

#include <gl/glew.h>
#include <glfw/glfw3.h>

#include <array>
#include <glm/vec2.hpp>

class MouseInputs {
  // Assertions to verify that button value doesn't change
  static_assert(GLFW_MOUSE_BUTTON_LEFT == 0);
  static_assert(GLFW_MOUSE_BUTTON_RIGHT == 1);
  static_assert(GLFW_MOUSE_BUTTON_MIDDLE == 2);
  static_assert(GLFW_MOUSE_BUTTON_4 == 3);
  static_assert(GLFW_MOUSE_BUTTON_5 == 4);
  static_assert(GLFW_MOUSE_BUTTON_6 == 5);
  static_assert(GLFW_MOUSE_BUTTON_7 == 6);
  static_assert(GLFW_MOUSE_BUTTON_LAST == 7);

 public:
  enum class ButtonState { kRelease, kPress, kHold, kNone };
  enum class ScrollState { kNone, kDown, kUp };
  enum class ButtonType : uint8_t {
    kLeft = GLFW_MOUSE_BUTTON_LEFT,
    kMiddle = GLFW_MOUSE_BUTTON_MIDDLE,
    kRight = GLFW_MOUSE_BUTTON_RIGHT,
    kLast = GLFW_MOUSE_BUTTON_LAST,
    kButton4 = GLFW_MOUSE_BUTTON_4,
    kButton5 = GLFW_MOUSE_BUTTON_5,
    kButton6 = GLFW_MOUSE_BUTTON_6,
    kButton7 = GLFW_MOUSE_BUTTON_7,
  };

  MouseInputs();
  ~MouseInputs();

  void Refresh() noexcept;

  void UpdateMouse(float x_pos, float y_pos) noexcept;
  void UpdateScroll(float x_offset, float y_offset) noexcept;
  void UpdateButton(int button, int action) noexcept;

  bool CheckButtonPress(ButtonType type) const noexcept;
  bool CheckButtonHold(ButtonType type) const noexcept;
  bool CheckButtonRelease(ButtonType type) const noexcept;

  glm::vec2 GetDisplacement() const noexcept;
  glm::vec2 GetPos() const noexcept;

  ScrollState GetScrollState() const noexcept;

 private:
  friend class InputManager;

  // 8 unique buttons
  static constexpr size_t kButtonAmount = 8;

  // Scroll Data
  ScrollState scroll_direction_ = ScrollState::kNone;

  // Mouse Data
  glm::vec2 displacement_ = {0.0f, 0.0f};
  glm::vec2 position_ = {0.0f, 0.0f};

  // Array to store the button data
  std::array<ButtonState, kButtonAmount> button_states_;
};
