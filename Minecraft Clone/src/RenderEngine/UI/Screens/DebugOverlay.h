// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <array>
#include <format>
#include <string>

#include "RenderEngine/UI/Screens/Screen.h"
#include "Utils/Timer/Timer.h"

class TextWidget;
class GameContext;
class ScreenManager;

// Debug overlay screen that displays version info and stats.
// Replaces the old DebugScreen implementation.
class DebugOverlay : public Screen {
 public:
  static constexpr int kNumStats = 10;

  // Constructor for use in the UI system screen stack
  DebugOverlay(GameContext& context, ScreenManager& screen_mgr);
  ~DebugOverlay() override;

  void OnEnter() override;
  void OnPause() override;
  void OnResume() override;
  void OnExit() override;

  void EditText(int idx, const std::string& text);

  // Formats and edits the text at the given index using std::format syntax
  template <typename... Args>
  void Edit(int idx, std::format_string<Args...> fmt, Args&&... args) {
    EditText(idx, std::format(fmt, std::forward<Args>(args)...));
  }

  int update_rate_ = 200;  // in milliseconds
  Timer timer;

 private:
  TextWidget* title_widget_ = nullptr;
  std::array<TextWidget*, kNumStats> stat_widgets_{};
};
