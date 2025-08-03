#include "Client/Inputs/MouseInputs.h"

MouseInputs::MouseInputs() { button_states_.fill(ButtonState::kRelease); }
MouseInputs::~MouseInputs() = default;

void MouseInputs::Refresh() noexcept {
  scroll_direction_ = ScrollState::kNone;

  displacement_ = glm::dvec2(0.0, 0.0);

  for (auto& state : button_states_) {
    if (state == ButtonState::kPress) {
      state = ButtonState::kHold;
    }
  }
}

void MouseInputs::UpdateMouse(float x_pos, float y_pos) noexcept {
  displacement_ = glm::vec2(x_pos, y_pos) - position_;
  position_ = glm::vec2(x_pos, y_pos);
}

void MouseInputs::UpdateScroll(float x_offset, float y_offset) noexcept {
  if (y_offset == 1.0f) {
    scroll_direction_ = ScrollState::kUp;
  }

  if (y_offset == -1.0f) {
    scroll_direction_ = ScrollState::kDown;
  }
  (void)x_offset;  // TODO(hiheyok): Manage this later
}

void MouseInputs::UpdateButton(int button, int action) noexcept {
  ButtonState state = ButtonState::kRelease;
  ButtonState alt_state = ButtonState::kHold;

  if (action == GLFW_PRESS) {
    state = ButtonState::kPress;
  } else {
    alt_state = ButtonState::kRelease;
  }

  // If the state is already kPress change it to kHold
  if (button_states_[button] == state) {
    button_states_[button] = alt_state;
  } else {
    button_states_[button] = state;
  }
}

bool MouseInputs::CheckButtonPress(ButtonType type) const noexcept {
  uint8_t idx = static_cast<uint8_t>(type);
  return button_states_[idx] == ButtonState::kPress;
}
bool MouseInputs::CheckButtonHold(ButtonType type) const noexcept {
  uint8_t idx = static_cast<uint8_t>(type);
  return button_states_[idx] == ButtonState::kHold;
}
bool MouseInputs::CheckButtonRelease(ButtonType type) const noexcept {
  uint8_t idx = static_cast<uint8_t>(type);
  return button_states_[idx] == ButtonState::kRelease;
}

glm::vec2 MouseInputs::GetDisplacement() const noexcept {
  return displacement_;
}

glm::vec2 MouseInputs::GetPos() const noexcept { return position_; }

MouseInputs::ScrollState MouseInputs::GetScrollState() const noexcept {
  return scroll_direction_;
}
