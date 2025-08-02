#include "RenderEngine/UI/ScreenManager.h"

#include "RenderEngine/UI/Screens/Screen.h"

ScreenManager::ScreenManager(GameContext& game_context,
                             ScreenRegistry& screen_registry)
    : game_context_{game_context}, screen_registry_{screen_registry} {}
ScreenManager::~ScreenManager() = default;

void ScreenManager::PopScreen() {
  assert(screens_.empty() && "No screens to pop");
  screens_.pop();
}

void ScreenManager::Update() {}

void ScreenManager::SubmitToRenderer(UIRenderer& ui_renderer) {
  screens_.top()->SubmitToRenderer(ui_renderer);
}
