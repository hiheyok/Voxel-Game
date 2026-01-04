#pragma once
#include <optional>

#include "Level/Item/Item.h"
#include "RenderEngine/UI/Widgets/Widget.h"

class Item;
class GameContext;

class SlotWidget : public Widget {
 public:
  SlotWidget(GameContext&);
  ~SlotWidget();

  void SetItem(const Item&);

 private:
  void Build();

  std::optional<Item> current_item_;
};
