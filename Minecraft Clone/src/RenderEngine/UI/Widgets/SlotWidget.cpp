#include "RenderEngine/UI/Widgets/SlotWidget.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Item/Item.h"
#include "RenderEngine/RenderResources/Manager/ItemIconManager.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/UI/Components/TextureComponent.h"
#include "RenderEngine/UI/Widgets/Widget.h"

using std::make_unique;
using std::move;

SlotWidget::SlotWidget(GameContext& context) : Widget{context} {}
SlotWidget::~SlotWidget() = default;

void SlotWidget::SetItem(const Item& item) {
  if (current_item_.has_value() && current_item_.value() == item) {
    return;
  }

  if (!components_.empty()) {
    components_.pop_back();
  }

  auto& manager = context_.render_resource_manager_->GetItemIconManager();
  TextureSprite sprite = manager.GetAddItemIcon(item);
  auto item_icon = make_unique<TextureComponent>(sprite);
  components_.push_back(move(item_icon));

  current_item_ = item;
}
