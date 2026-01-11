#pragma once

#include <memory>
#include <stack>

#include "Client/Inputs/InputEvent.h"
#include "RenderEngine/UI/ScreenRegistry.h"
#include "RenderEngine/UI/Screens/Screen.h"

class Screen;
class GameContext;
class UIRenderer;

class ScreenManager {
 public:
  ScreenManager(GameContext&, ScreenRegistry&);
  ~ScreenManager();

  // returns the screen index on the stack
  template <typename... Args>
  int PushScreen(const std::string& name, Args&&... args);
  template <typename... Args>
  void SwitchScreen(const std::string& name, Args&&... args);
  void PopScreen();
  void Update(const std::vector<InputEvent>& events);
  void SubmitToRenderer(UIRenderer&);
  void ChangeVirtualRes(glm::vec2 v_res);
  void SetScreenTickCallback(int idx, Screen::TickCallback callback);

  // Get the current top screen (for external access)
  Screen* GetTopScreen();

  const std::vector<std::unique_ptr<Screen>>& GetScreens() const noexcept;

 private:
  GameContext& context_;
  ScreenRegistry& screen_registry_;
  std::vector<std::unique_ptr<Screen>> screens_;
  float ui_scaling_;
};

template <typename... Args>
int ScreenManager::PushScreen(const std::string& name, Args&&... args) {
  std::unique_ptr<Screen> screen =
      screen_registry_.CreateScreen(name, *this, std::forward<Args>(args)...);
  screen->OnEnter();
  screens_.emplace_back(std::move(screen));
  return screens_.size() - 1;
}

template <typename... Args>
void ScreenManager::SwitchScreen(const std::string& name, Args&&... args) {
  PopScreen();
  PushScreen(*this, std::forward<Args>(args)...);
}