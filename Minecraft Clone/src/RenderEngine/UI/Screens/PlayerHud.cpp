#include "RenderEngine/UI/Screens/PlayerHud.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Item/Items.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2D.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"
#include "RenderEngine/UI/Components/ColoredComponent.h"
#include "RenderEngine/UI/Components/Component.h"
#include "RenderEngine/UI/Components/TextureComponent.h"
#include "RenderEngine/UI/Widgets/SlotWidget.h"
#include "RenderEngine/UI/Widgets/Widget.h"

using std::make_unique;
using std::move;

PlayerHud::PlayerHud(GameContext& context, ScreenManager& screen_mgr)
    : Screen{context, screen_mgr, {1920.0f, 1080.0f}} {
  root_widget_->SetAnchorBoth({0.0f, 0.0f});
  root_widget_->SetPivot({0.0f, 0.0f});
  root_widget_->SetOffsetMax(GetVirtualRes());
}
PlayerHud::~PlayerHud() = default;

void PlayerHud::OnEnter() {
  RenderResourceManager& resources = *context_.render_resource_manager_;
  RenderHandle<TextureAtlas> gui = resources.GetAtlas("gui");

  auto hotbar_tex =
      make_unique<TextureComponent>(gui->GetSprite("minecraft:hud/hotbar"));

  auto clr = make_unique<ColoredComponent>(1.0f, 0.0f, 1.0f);

  auto hotbar = make_unique<Widget>(context_);

  auto slot = make_unique<SlotWidget>(context_);
  slot->SetItem(context_.items_->ACACIA_LOG);
  slot->SetAnchorBoth({0.0f, 0.0f});
  slot->SetPivot({0.0f, 0.0f});
  slot->SetOffsetMax({22.0f, 22.0f});

  hotbar->SetAnchorBoth({0.5f, 0.0f});
  hotbar->SetOffsetMax({182.0f, 22.0f});
  hotbar->SetPivot({0.5f, 0.0f});
  hotbar->AddComponent(move(hotbar_tex));

  hotbar->AddChildWidget(move(slot));
  root_widget_->AddChildWidget(move(hotbar));
}
void PlayerHud::OnPause() {}
void PlayerHud::OnResume() {}
void PlayerHud::OnExit() {}