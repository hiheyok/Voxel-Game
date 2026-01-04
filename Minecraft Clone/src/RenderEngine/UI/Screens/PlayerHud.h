#pragma once

#include <array>

#include "RenderEngine/UI/Screens/Screen.h"

class SlotWidget;
class Widget;
class Item;

class PlayerHud : public Screen {
 public:
  explicit PlayerHud(GameContext&, ScreenManager&);
  ~PlayerHud() override;

  // Initialize all of the widgets, component etc
  void OnEnter() override;
  void OnPause() override;
  void OnResume() override;
  // Clean up before exiting, saving state, etc
  void OnExit() override;

  void Update(const std::vector<InputEvent>& events) override;

  // Called by tick callback to update slot contents
  void UpdateSlot(int index, const Item& item);
  void UpdateSelectedSlot(int index);

 private:
  static constexpr int kHotbarSlots = 9;
  std::array<SlotWidget*, kHotbarSlots> hotbar_slots_{};
  Widget* selection_indicator_ = nullptr;
  int current_slot_index_ = 0;
};
