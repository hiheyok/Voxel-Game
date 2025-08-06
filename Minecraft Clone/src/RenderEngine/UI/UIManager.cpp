#include "RenderEngine/UI/UIManager.h"

#include <cassert>

#include "Client/Inputs/InputManager.h"
#include "RenderEngine/UI/ScreenManager.h"
#include "RenderEngine/UI/ScreenRegistry.h"
#include "RenderEngine/UI/Screens/MainScreen.h"
#include "RenderEngine/UI/UIRenderer.h"

UIManager::UIManager(GameContext& game_context, InputManager& input_mgr)
    : game_context_{game_context},
      input_mgr_{input_mgr},
      renderer_{std::make_unique<UIRenderer>(game_context)},
      screen_registry_{std::make_unique<ScreenRegistry>()},
      screen_manager_{
          std::make_unique<ScreenManager>(game_context, *screen_registry_)} {}

UIManager::~UIManager() = default;

void UIManager::Initialize() {
  assert(screen_registry_ != nullptr &&
         "Screen Registry hasn't been created yet");
  screen_registry_->RegisterScreen<MainScreen, ScreenManager&>("debug_screen");
}

void UIManager::PopScreen() { screen_manager_->PopScreen(); }

void UIManager::Render() {
  assert(renderer_ && screen_manager_);
  renderer_->Clear();
  screen_manager_->SubmitToRenderer(*renderer_);
  renderer_->Render();
}

void UIManager::Update() { screen_manager_->Update(input_mgr_.GetEvents()); }

void UIManager::ScreenResChanged(glm::vec2 res) {
  if (curr_res_ != res) {
    curr_res_ = res;
    renderer_->SetScreenRes(res);
    screen_manager_->ChangeVirtualRes(res);
  }
}
