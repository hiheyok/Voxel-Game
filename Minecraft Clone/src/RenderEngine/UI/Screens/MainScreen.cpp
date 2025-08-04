#include "RenderEngine/UI/Screens/MainScreen.h"

#include "RenderEngine/UI/Components/ColoredComponent.h"
#include "RenderEngine/UI/Components/Component.h"
#include "RenderEngine/UI/Widgets/Widget.h"

MainScreen::MainScreen(ScreenManager& screen_mgr) : Screen{screen_mgr} {}
MainScreen::~MainScreen() = default;

void MainScreen::OnEnter() {
  auto red = std::make_unique<ColoredComponent>(1.0f, 0.0f, 0.0f, 0.0f);
  auto green = std::make_unique<ColoredComponent>(0.0f, 1.0f, 0.0f, 0.5f);
  auto blue = std::make_unique<ColoredComponent>(0.0f, 0.0f, 1.0f, 0.5f);
  auto blue1 = std::make_unique<ColoredComponent>(0.0f, 1.0f, 1.0f, 0.5f);
  root_widget_->SetOffsetMax(1.0f, 1.0f);
  root_widget_->SetAnchorBoth(0.0f, 0.0f);
  root_widget_->SetPivot(0.0f, 0.0f);
  root_widget_->AddComponent(std::move(red));

  auto widget = std::make_unique<Widget>();
  widget->SetOffsetMax(0.5f, 0.5f);
  widget->SetAnchorBoth(0.0f, 0.0f);
  widget->SetPivot(0.0f, 0.0f);
  widget->AddComponent(std::move(green));

  auto button = std::make_unique<Widget>();
  button->SetOffsetMax(0.5f, 0.5f);
  button->SetAnchorBoth(0.0f, 0.0f);
  button->SetPivot(0.0f, 0.0f);
  button->AddComponent(std::move(blue));

  auto button2 = std::make_unique<Widget>();
  button2->SetOffsetMax(0.25f, 0.25f);
  button2->SetAnchorBoth(1.0f, 1.0f);
  button2->SetPivot(1.0f, 1.0f);
  button2->AddComponent(std::move(blue1));

  button->AddChildWidget(std::move(button2));
  widget->AddChildWidget(std::move(button));
  root_widget_->AddChildWidget(std::move(widget));
}
void MainScreen::OnPause() {}
void MainScreen::OnResume() {}
void MainScreen::OnExit() {}