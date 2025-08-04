#pragma once

#include <memory>

#include "RenderEngine/UI/ScreenManager.h"

class UIRenderer;
class ScreenRegistry;
class ScreenManager;
class GameContext;
class InputManager;

class UIManager {
 public:
  UIManager(GameContext&, InputManager& input_mgr);
  ~UIManager();

  // Populate the screen registry
  void Initialize();

  // External api functions
  template <typename... Args>
  void PushScreen(const std::string& name, Args&&... args);

  template <typename... Args>
  void SwitchScreen(const std::string& name, Args&&... args);

  void PopScreen();

  void Render();

  void Update();

 private:
  GameContext& game_context_;
  InputManager& input_mgr_;

  std::unique_ptr<UIRenderer> renderer_;
  std::unique_ptr<ScreenRegistry> screen_registry_;
  std::unique_ptr<ScreenManager> screen_manager_;
};

// External api functions
template <typename... Args>
void UIManager::PushScreen(const std::string& name, Args&&... args) {
  screen_manager_->PushScreen(name, std::forward<Args>(args)...);
}

template <typename... Args>
void UIManager::SwitchScreen(const std::string& name, Args&&... args) {
  screen_manager_->SwitchScreen(name, std::forward<Args>(args)...);
}