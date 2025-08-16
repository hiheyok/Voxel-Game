#include "RenderEngine/UI/ScreenManager.h"

#include "RenderEngine/UI/Screens/Screen.h"
#include "RenderEngine/UI/UIRenderer.h"

ScreenManager::ScreenManager(GameContext& context,
                             ScreenRegistry& screen_registry)
    : context_{context}, screen_registry_{screen_registry}, ui_scaling_{3.0f} {}
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
  screens_.top()->SetVirtualRes(v_res / ui_scaling_);
}
