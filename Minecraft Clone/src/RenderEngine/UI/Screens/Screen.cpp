#include "RenderEngine/UI/Screens/Screen.h"

#include "RenderEngine/UI/Data/UserInterfaceData.h"
#include "RenderEngine/UI/Widgets/Widget.h"

Screen::Screen(ScreenManager& screen_mgr)
    : screen_mgr_{screen_mgr}, root_widget_{std::make_unique<Widget>()} {
  root_widget_->manager_ = &screen_mgr;
}
Screen::~Screen() { OnExit(); }

void Screen::SetVirtualResolution(glm::ivec2 v_res) { v_res_ = v_res; }
void Screen::HandleEvent() {
  // Implement later
}
void Screen::Update() {
  // Implement later
}

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

void Screen::OnEnter() {}
void Screen::OnPause() {}
void Screen::OnResume() {}
void Screen::OnExit() {}
