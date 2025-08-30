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
};
