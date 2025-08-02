#pragma once

#include "RenderEngine/UI/Screens/Screen.h"

class MainScreen : public Screen {
 public:
  explicit MainScreen(ScreenManager&);
  ~MainScreen() override;

  // Initialize all of the widgets, component etc
  void OnEnter() override;
  void OnPause() override;
  void OnResume() override;
  // Clean up before exiting, saving state, etc
  void OnExit() override;
};
