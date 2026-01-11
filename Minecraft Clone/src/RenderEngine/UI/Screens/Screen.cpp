#include "RenderEngine/UI/Screens/Screen.h"

#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <utility>
#include <vector>

#include "Client/Inputs/InputEvent.h"
#include "RenderEngine/UI/Data/UserInterfaceData.h"
#include "RenderEngine/UI/UIRenderer.h"
#include "RenderEngine/UI/Widgets/Widget.h"
#include "Utils/Assert.h"

using std::make_unique;
using std::move;
using std::vector;

Screen::Screen(GameContext& context, ScreenManager& screen_mgr, glm::vec2 v_res)
    : context_{context},
      screen_mgr_{screen_mgr},
      v_res_{v_res},
      root_widget_{make_unique<Widget>(context)},
      input_blocking_{false} {
  root_widget_->manager_ = &screen_mgr;
}

Screen::~Screen() { OnExit(); }

void Screen::HandleEvent() {
  // Implement later
}
void Screen::Update(const vector<InputEvent>& events) {}

void Screen::SetTickCallback(TickCallback callback) {
  tick_callback_ = move(callback);
}

void Screen::CallCallback() {
  if (tick_callback_ == nullptr) {
    return;
  }

  tick_callback_(this);
}

void Screen::UpdateResolution() {
  GAME_ASSERT(root_widget_, "Root widget is null");
  UIRectangle bounds;
  bounds.pos_.x = 0.0f;
  bounds.pos_.y = 0.0f;
  // Use virtual resolution for bounds, not normalized coordinates
  bounds.size_ = v_res_;

  root_widget_->TryUpdateLayout(bounds);
}

void Screen::SubmitToRenderer(UIRenderer& renderer) const {
  GAME_ASSERT(root_widget_, "Root widget is null");
  root_widget_->SubmitToRenderer(renderer);
}

glm::vec2 Screen::GetVirtualRes() const noexcept { return v_res_; }

void Screen::SetVirtualRes(glm::vec2 v_res) {
  GAME_ASSERT(v_res != v_res_, "Vertical resolution unchanged");
  v_res_ = v_res;
  root_widget_->SetOffsetMax(v_res);
  root_widget_->SetBranchDirty();
}

void Screen::OnEnter() {}
void Screen::OnPause() {}
void Screen::OnResume() {}
void Screen::OnExit() {}

bool Screen::IsInputBlocking() const noexcept { return input_blocking_; }

void Screen::TryUpdateLayout() {
  UIRectangle bounds;
  bounds.pos_.x = 0.0f;
  bounds.pos_.y = 0.0f;
  bounds.size_ = v_res_;
  root_widget_->TryUpdateLayout(bounds);
}
