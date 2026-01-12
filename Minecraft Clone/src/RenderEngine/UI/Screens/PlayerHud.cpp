#include "RenderEngine/UI/Screens/PlayerHud.h"

#include <vector>

#include "Client/Inputs/InputEvent.h"
#include "Core/GameContext/GameContext.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Item/Item.h"
#include "Level/Item/ItemStack.h"
#include "Level/Item/Items.h"
#include "RenderEngine/FrameBuffer/RenderTargetTexture.h"
#include "RenderEngine/RenderResources/Manager/ItemIconManager.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"
#include "RenderEngine/UI/Components/ColoredComponent.h"
#include "RenderEngine/UI/Components/TextureComponent.h"
#include "RenderEngine/UI/Screens/Screen.h"
#include "RenderEngine/UI/Widgets/SlotWidget.h"
#include "RenderEngine/UI/Widgets/Widget.h"
#include "Utils/Assert.h"

using std::make_unique;
using std::move;
using std::vector;

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

  auto& manager = context_.render_resource_manager_->GetItemIconManager();

  // auto test_tex = make_unique<TextureComponent>(manager.GetAtlasTexture());

  auto hotbar = make_unique<Widget>(context_);
  hotbar->SetAnchorBoth({0.5f, 0.0f});
  hotbar->SetOffsetMax({182.0f, 22.0f});
  hotbar->SetPivot({0.5f, 0.0f});
  hotbar->AddComponent(move(hotbar_tex));

  // Create 9 hotbar slots
  for (int i = 0; i < kHotbarSlots; i++) {
    auto slot = make_unique<SlotWidget>(context_);
    slot->SetAnchorBoth({0.0f, 0.0f});
    slot->SetPivot({0.0f, 0.0f});
    slot->SetOffsetMin({3.0f + i * 20.0f, 3.0f});
    slot->SetOffsetMax({19.0f + i * 20.0f, 19.0f});

    hotbar_slots_[i] = slot.get();
    hotbar->AddChildWidget(move(slot));
  }

  // Selection indicator
  auto selector_tex = make_unique<TextureComponent>(
      gui->GetSprite("minecraft:hud/hotbar_selection"));
  auto selector = make_unique<Widget>(context_);
  selector->SetAnchorBoth({0.0f, 0.0f});
  selector->SetPivot({0.0f, 0.0f});
  selector->SetOffsetMin({-1.0f, -1.0f});
  selector->SetOffsetMax({23.0f, 23.0f});
  selector->AddComponent(move(selector_tex));
  selection_indicator_ = selector.get();
  hotbar->AddChildWidget(move(selector));
  root_widget_->AddChildWidget(move(hotbar));

  // Crosshair
  auto crosshair_tex =
      make_unique<TextureComponent>(gui->GetSprite("minecraft:hud/crosshair"));
  auto crosshair = make_unique<Widget>(context_);
  crosshair->SetAnchorBoth({0.5f, 0.5f});
  crosshair->SetPivot({0.5f, 0.5f});
  crosshair->SetOffsetMin({0.0f, 0.0f});
  crosshair->SetOffsetMax({15.0f, 15.0f});
  crosshair->AddComponent(move(crosshair_tex));
  root_widget_->AddChildWidget(move(crosshair));
}

void PlayerHud::OnPause() {}
void PlayerHud::OnResume() {}
void PlayerHud::OnExit() {}

void PlayerHud::Update(const vector<InputEvent>& events) {
  Screen::Update(events);

  if (!context_.main_player_) return;

  auto& inventory = context_.main_player_->entity_inventory_;

  // Update slot contents
  for (int i = 0; i < kHotbarSlots; i++) {
    ItemStack item = inventory.GetItem(i);
    if (item.IsInitialized()) {
      hotbar_slots_[i]->SetItem(item.item_);
    }
  }

  // Update selection indicator position
  int slot = inventory.right_hand_slot_;
  UpdateSelectedSlot(slot);
}

void PlayerHud::UpdateSlot(int index, const Item& item) {
  GAME_ASSERT(index >= 0 && index < kHotbarSlots, "Invalid hotbar bounds.");
  hotbar_slots_[index]->SetItem(item);
}

void PlayerHud::UpdateSelectedSlot(int index) {
  if (index == current_slot_index_) return;
  current_slot_index_ = index;
  selection_indicator_->SetOffsetMin({-1.0f + index * 20.0f, -1.0f});
  selection_indicator_->SetOffsetMax({23.0f + index * 20.0f, 23.0f});
  selection_indicator_->SetDirty();
}