#include "RenderEngine/UI/Screens/MainScreen.h"

#include "RenderEngine/UI/Components/ColoredComponent.h"
#include "RenderEngine/UI/Components/Component.h"
#include "RenderEngine/UI/Widgets/Widget.h"

MainScreen::MainScreen(ScreenManager& screen_mgr) : Screen{screen_mgr} {}
MainScreen::~MainScreen() = default;

void MainScreen::OnEnter() {
  std::unique_ptr<ColoredComponent> component =
      std::make_unique<ColoredComponent>(0.5f, 0.6f, 0.8f);
  root_widget_->SetOffsetMax(0.5f, 0.5f);
  root_widget_->AddComponent(std::move(component));
}
void MainScreen::OnPause() {}
void MainScreen::OnResume() {}
void MainScreen::OnExit() {}