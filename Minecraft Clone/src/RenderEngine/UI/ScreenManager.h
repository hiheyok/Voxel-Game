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

  template <typename... Args>
  void PushScreen(const std::string& name, Args&&... args);
  template <typename... Args>
  void SwitchScreen(const std::string& name, Args&&... args);
  void PopScreen();
  void Update(const std::vector<InputEvent>& events);
  void SubmitToRenderer(UIRenderer&);

 private:
  GameContext& game_context_;
  ScreenRegistry& screen_registry_;
  std::stack<std::unique_ptr<Screen>> screens_;
};

template <typename... Args>
void ScreenManager::PushScreen(const std::string& name, Args&&... args) {
  std::unique_ptr<Screen> screen =
      screen_registry_.CreateScreen(name, *this, std::forward<Args>(args)...);
  screen->OnEnter();
  screens_.push(std::move(screen));
}
template <typename... Args>
void ScreenManager::SwitchScreen(const std::string& name, Args&&... args) {
  PopScreen();
  PushScreen(*this, std::forward<Args>(args)...);
}