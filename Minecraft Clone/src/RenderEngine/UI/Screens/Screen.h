// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <functional>
#include <glm/vec2.hpp>
#include <memory>
#include <vector>

#include "Client/Inputs/InputEvent.h"

class Widget;
class ScreenManager;
class UIRenderer;
class GameContext;

class Screen {
 public:
  virtual ~Screen();

  Screen(const Screen&) = delete;
  Screen& operator=(const Screen&) = delete;
  Screen(Screen&&) = delete;
  Screen& operator=(Screen&&) = delete;

  // input event
  void HandleEvent();
  using TickCallback = std::function<void(Screen*)>;

  void UpdateResolution();
  // input delta time to update stuff like animation
  virtual void Update(const std::vector<InputEvent>& events);

  void SetTickCallback(TickCallback callback);
  void CallCallback();
  void SubmitToRenderer(UIRenderer& renderer) const;

  glm::vec2 GetVirtualRes() const noexcept;
  void SetVirtualRes(glm::vec2 v_res);

  // Initialize all of the widgets, component etc
  virtual void OnEnter();
  virtual void OnPause();
  virtual void OnResume();
  // Clean up before exiting, saving state, etc
  virtual void OnExit();

  bool IsInputBlocking() const noexcept;
  void TryUpdateLayout();

 protected:
  explicit Screen(GameContext&, ScreenManager&, glm::vec2 virtual_res);

  GameContext& context_;
  glm::vec2 v_res_;

  ScreenManager& screen_mgr_;
  std::unique_ptr<Widget> root_widget_;
  TickCallback tick_callback_;
  // Stops inputs from propagating
  bool input_blocking_;
};
