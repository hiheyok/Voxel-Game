// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec2.hpp>
#include <memory>

class Widget;
class ScreenManager;
class UIRenderer;

class Screen {
 public:
  explicit Screen(ScreenManager&);
  virtual ~Screen();

  Screen(const Screen&) = delete;
  Screen& operator=(const Screen&) = delete;
  Screen(Screen&&) = delete;
  Screen& operator=(Screen&&) = delete;

  void SetVirtualResolution(glm::ivec2 v_res);
  // input event
  void HandleEvent();
  // input delta time to update stuff like animation
  void Update();

  void SubmitToRenderer(UIRenderer& renderer);

  // Initialize all of the widgets, component etc
  virtual void OnEnter();
  virtual void OnPause();
  virtual void OnResume();
  // Clean up before exiting, saving state, etc
  virtual void OnExit();

 protected:
  ScreenManager& screen_mgr_;
  std::unique_ptr<Widget> root_widget_;
  glm::ivec2 v_res_;
};
