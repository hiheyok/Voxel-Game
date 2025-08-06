#include "RenderEngine/UI/ScreenManager.h"

#include "RenderEngine/UI/Screens/Screen.h"
#include "RenderEngine/UI/UIRenderer.h"

ScreenManager::ScreenManager(GameContext& game_context,
                             ScreenRegistry& screen_registry)
    : game_context_{game_context}, screen_registry_{screen_registry} {}
ScreenManager::~ScreenManager() = default;

void ScreenManager::PopScreen() {
  assert(screens_.empty() && "No screens to pop");
  screens_.pop();
}

void ScreenManager::Update(const std::vector<InputEvent>& events) {
  screens_.top()->Update(events);
}

void ScreenManager::SubmitToRenderer(UIRenderer& ui_renderer) {
  screens_.top()->SubmitToRenderer(ui_renderer);
  ui_renderer.SetVirtualRes(screens_.top()->GetVirtualRes());
}

void ScreenManager::ChangeVirtualRes(glm::vec2 v_res) {
  screens_.top()->ChangeVirtualRes(v_res);
}
