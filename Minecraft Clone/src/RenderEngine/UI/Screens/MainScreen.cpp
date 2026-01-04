#include "RenderEngine/UI/Screens/MainScreen.h"

#include <memory>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2D.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"
#include "RenderEngine/UI/Components/ColoredComponent.h"
#include "RenderEngine/UI/Components/TextureComponent.h"
#include "RenderEngine/UI/Screens/Screen.h"
#include "RenderEngine/UI/Widgets/Widget.h"

using std::make_unique;
using std::move;

MainScreen::MainScreen(GameContext& context, ScreenManager& screen_mgr)
    : Screen{context, screen_mgr, {1920.0f, 1080.0f}} {
  root_widget_->SetAnchorBoth({0.0f, 0.0f});
  root_widget_->SetPivot({0.0f, 0.0f});
  root_widget_->SetOffsetMax(GetVirtualRes());
}
MainScreen::~MainScreen() = default;

void MainScreen::OnEnter() {
  auto red = make_unique<ColoredComponent>(1.0f, 0.0f, 0.0f, 0.5f);
  auto green = make_unique<ColoredComponent>(0.0f, 1.0f, 0.0f, 0.5f);
  auto blue = make_unique<ColoredComponent>(0.0f, 0.0f, 1.0f, 0.5f);
  auto blue1 = make_unique<ColoredComponent>(0.0f, 1.0f, 1.0f, 0.5f);

  auto texture = make_unique<TextureComponent>(
      context_.render_resource_manager_->GetTexture2D("widgets"));

  auto atlas = make_unique<TextureComponent>(
      context_.render_resource_manager_->GetAtlas("gui"));

  auto widget1 = make_unique<Widget>(context_);
  widget1->SetAnchorMin({0.0f, 0.0f});
  widget1->SetAnchorMax({1.0f, 1.0f});
  widget1->SetPivot({0.0f, 0.0f});
  widget1->AddComponent(move(red));
  widget1->AddComponent(move(atlas));

  root_widget_->AddChildWidget(move(widget1));
}
void MainScreen::OnPause() {}
void MainScreen::OnResume() {}
void MainScreen::OnExit() {}