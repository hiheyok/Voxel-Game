#include "RenderEngine/UI/ScreenManager.h"

#include <glm/ext/vector_float2.hpp>
#include <utility>
#include <vector>

#include "Client/Inputs/InputEvent.h"
#include "RenderEngine/UI/ScreenRegistry.h"
#include "RenderEngine/UI/Screens/Screen.h"
#include "RenderEngine/UI/UIRenderer.h"
#include "Utils/Assert.h"

using std::move;
using std::unique_ptr;
using std::vector;

ScreenManager::ScreenManager(GameContext& context,
                             ScreenRegistry& screen_registry)
    : context_{context}, screen_registry_{screen_registry}, ui_scaling_{3.0f} {}
ScreenManager::~ScreenManager() = default;

void ScreenManager::PopScreen() {
  GAME_ASSERT(screens_.empty(), "No screens to pop");
  screens_.pop_back();
}

void ScreenManager::Update(const vector<InputEvent>& events) {
  GAME_ASSERT(!screens_.empty(), "No screen to update.");

  // Calls the screenn callback func if it has one
  for (auto& screen : screens_) {
    screen->CallCallback();
  }

  for (size_t i = screens_.size(); i-- > 0; ) {
    screens_[i]->Update(events);
    screens_[i]->TryUpdateLayout();
    if (screens_[i]->IsInputBlocking()) {
      break;
    }
  }
}

void ScreenManager::SubmitToRenderer(UIRenderer& ui_renderer) {
  for (const auto& screen : screens_) {
    ui_renderer.SetVirtualRes(screen->GetVirtualRes());
    screen->SubmitToRenderer(ui_renderer);
  }
}

void ScreenManager::ChangeVirtualRes(glm::vec2 v_res) {
  for (auto& screen : screens_) {
    screen->SetVirtualRes(v_res / ui_scaling_);
    screen->UpdateResolution();
  }
}

void ScreenManager::SetScreenTickCallback(int idx, Screen::TickCallback callback) {
  GAME_ASSERT(idx >= 0 && idx < screens_.size(), "Index out of bounds.");

  screens_[idx]->SetTickCallback(move(callback));
}

Screen* ScreenManager::GetTopScreen() {
  if (screens_.empty()) return nullptr;
  return screens_.back().get();
}

const vector<unique_ptr<Screen>>& ScreenManager::GetScreens() const noexcept {
  return screens_;
}
