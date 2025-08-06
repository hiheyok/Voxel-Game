#include "RenderEngine/UI/Screens/MainScreen.h"

#include "RenderEngine/UI/Components/ColoredComponent.h"
#include "RenderEngine/UI/Components/Component.h"
#include "RenderEngine/UI/Widgets/Widget.h"

MainScreen::MainScreen(ScreenManager& screen_mgr) : Screen{screen_mgr, {1920.0f, 1080.0f}} {
  root_widget_->SetAnchorBoth({0.0f, 0.0f});
  root_widget_->SetPivot({0.0f, 0.0f});
  root_widget_->SetOffsetMax(GetVirtualRes());
}
MainScreen::~MainScreen() = default;

void MainScreen::OnEnter() {
  auto red = std::make_unique<ColoredComponent>(1.0f, 0.0f, 0.0f, 0.5f);
  auto green = std::make_unique<ColoredComponent>(0.0f, 1.0f, 0.0f, 0.5f);
  auto blue = std::make_unique<ColoredComponent>(0.0f, 0.0f, 1.0f, 0.5f);
  auto blue1 = std::make_unique<ColoredComponent>(0.0f, 1.0f, 1.0f, 0.5f);

  auto widget1 = std::make_unique<Widget>();
  widget1->SetAnchorMin({0.0f, 0.0f});
  widget1->SetAnchorMax({0.5f, 0.5f});
  widget1->SetPivot({0.0f, 0.0f});
  widget1->AddComponent(std::move(green));

  root_widget_->AddChildWidget(std::move(widget1));
}
void MainScreen::OnPause() {}
void MainScreen::OnResume() {}
void MainScreen::OnExit() {}