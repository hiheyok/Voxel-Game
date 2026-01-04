#include "RenderEngine/UI/Screens/Screen.h"

#include "RenderEngine/UI/Data/UserInterfaceData.h"
#include "RenderEngine/UI/Widgets/Widget.h"
#include "RenderEngine/UI/UIRenderer.h"
#include "Utils/Assert.h"

Screen::Screen(GameContext& context, ScreenManager& screen_mgr, glm::vec2 v_res)
    : context_{context},
      screen_mgr_{screen_mgr},
      v_res_{v_res},
      root_widget_{std::make_unique<Widget>(context)} {
  root_widget_->manager_ = &screen_mgr;
}
Screen::~Screen() { OnExit(); }

void Screen::HandleEvent() {
  // Implement later
}
void Screen::Update(const std::vector<InputEvent>& events) {
  if (tick_callback_) tick_callback_();
}

void Screen::SetTickCallback(TickCallback callback) {
  tick_callback_ = std::move(callback);
}

void Screen::SubmitToRenderer(UIRenderer& renderer) {
  GAME_ASSERT(root_widget_, "Root widget is null");
  UIRectangle bounds;
  bounds.pos_.x = 0.0f;
  bounds.pos_.y = 0.0f;
  bounds.size_.x = 1.0f;
  bounds.size_.y = 1.0f;

  root_widget_->TryUpdateLayout(bounds);
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
