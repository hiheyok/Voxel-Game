#include "RenderEngine/UI/Widgets/SlotWidget.h"

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/RenderResources/Manager/ItemIconManager.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/UI/Components/TextureComponent.h"

using std::make_unique;
using std::move;

SlotWidget::SlotWidget(GameContext& context) : Widget{context} {}
SlotWidget::~SlotWidget() = default;

void SlotWidget::SetItem(const Item& item) {
  if (!components_.empty()) {
    components_.pop_back();
  }

  auto& manager = context_.render_resource_manager_->GetItemIconManager();
  TextureSprite sprite = manager.GetAddItemIcon(item);

  auto item_icon = make_unique<TextureComponent>(sprite);
  components_.push_back(move(item_icon));
}
