#pragma once

#include "RenderEngine/UI/Screens/Screen.h"

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
};
