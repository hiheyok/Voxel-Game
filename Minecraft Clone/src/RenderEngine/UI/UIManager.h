#pragma once

#include <memory>

#include "RenderEngine/UI/ScreenManager.h"

class UIRenderer;
class ScreenRegistry;
class ScreenManager;
class GameContext;

class UIManager {
 public:
  explicit UIManager(GameContext&);
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

 private:
  GameContext& game_context_;

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