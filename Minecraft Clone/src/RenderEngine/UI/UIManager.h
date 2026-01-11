#pragma once

#include <glm/vec2.hpp>
#include <memory>

#include "RenderEngine/UI/ScreenManager.h"

class UIRenderer;
class ScreenRegistry;
class ScreenManager;
class GameContext;
class InputManager;

class UIManager {
 public:
  explicit UIManager(GameContext&, InputManager& input_mgr);
  ~UIManager();

  // Populate the screen registry
  void Initialize();

  // External api functions
  template <typename... Args>
  int PushScreen(const std::string& name, Args&&... args);

  template <typename... Args>
  void SwitchScreen(const std::string& name, Args&&... args);

  void PopScreen();

  void Render();

  void Update();

  void SetScreenTickCallback(int idx, Screen::TickCallback callback);

  void ScreenResChanged(glm::vec2 res);

 private:
  GameContext& context_;
  InputManager& input_mgr_;

  glm::vec2 curr_res_ = {0.0f, 0.0f};

  std::unique_ptr<UIRenderer> renderer_;
  std::unique_ptr<ScreenRegistry> screen_registry_;
  std::unique_ptr<ScreenManager> screen_manager_;
};

// External api functions
template <typename... Args>
int UIManager::PushScreen(const std::string& name, Args&&... args) {
  return screen_manager_->PushScreen(name, std::forward<Args>(args)...);
}

template <typename... Args>
void UIManager::SwitchScreen(const std::string& name, Args&&... args) {
  screen_manager_->SwitchScreen(name, std::forward<Args>(args)...);
}