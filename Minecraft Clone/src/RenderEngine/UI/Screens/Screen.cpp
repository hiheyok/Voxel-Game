#include "RenderEngine/UI/Screens/Screen.h"

#include "RenderEngine/UI/Data/UserInterfaceData.h"
#include "RenderEngine/UI/Widgets/Widget.h"

Screen::Screen(ScreenManager& screen_mgr, glm::vec2 v_res)
    : screen_mgr_{screen_mgr},
      v_res_{v_res},
      root_widget_{std::make_unique<Widget>()} {
  root_widget_->manager_ = &screen_mgr;
}
Screen::~Screen() { OnExit(); }

void Screen::HandleEvent() {
  // Implement later
}
void Screen::Update(const std::vector<InputEvent>& events) {}

void Screen::SubmitToRenderer(UIRenderer& renderer) {
  assert(root_widget_);
  UIRectangle bounds;
  bounds.pos_.x = 0.0f;
  bounds.pos_.y = 0.0f;
  bounds.size_.x = 1.0f;
  bounds.size_.y = 1.0f;

  root_widget_->TryUpdateLayout(bounds);
  root_widget_->SubmitToRenderer(renderer);
}

glm::vec2 Screen::GetVirtualRes() const noexcept { return v_res_; }

void Screen::ChangeVirtualRes(glm::vec2 v_res) {
  assert(v_res != v_res_);
  v_res_ = v_res;
  root_widget_->SetOffsetMax(v_res);
  root_widget_->SetBranchDirty();
}

void Screen::OnEnter() {}
void Screen::OnPause() {}
void Screen::OnResume() {}
void Screen::OnExit() {}
