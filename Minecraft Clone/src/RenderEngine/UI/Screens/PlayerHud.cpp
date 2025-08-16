#include "RenderEngine/UI/Screens/PlayerHud.h"

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2D.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"
#include "RenderEngine/UI/Components/ColoredComponent.h"
#include "RenderEngine/UI/Components/Component.h"
#include "RenderEngine/UI/Components/TextureComponent.h"
#include "RenderEngine/UI/Widgets/Widget.h"

PlayerHud::PlayerHud(GameContext& context, ScreenManager& screen_mgr)
    : Screen{context, screen_mgr, {1920.0f, 1080.0f}} {
  root_widget_->SetAnchorBoth({0.0f, 0.0f});
  root_widget_->SetPivot({0.0f, 0.0f});
  root_widget_->SetOffsetMax(GetVirtualRes());
}
PlayerHud::~PlayerHud() = default;

void PlayerHud::OnEnter() {
  RenderResourceManager& resources = *context_.render_resource_manager_;
  RenderHandle<TextureAtlasV2> gui = resources.GetAtlas("gui");

  auto hotbar_tex = std::make_unique<TextureComponent>(
      gui->GetSprite("minecraft:hud/hotbar"));

  auto hotbar = std::make_unique<Widget>();

  hotbar->SetAnchorBoth({0.5f, 0.0f});
  hotbar->SetOffsetMax({182.0f, 22.0f});

  hotbar->SetPivot({0.5f, 0.0f});
  hotbar->AddComponent(std::move(hotbar_tex));

  root_widget_->AddChildWidget(std::move(hotbar));
}
void PlayerHud::OnPause() {}
void PlayerHud::OnResume() {}
void PlayerHud::OnExit() {}