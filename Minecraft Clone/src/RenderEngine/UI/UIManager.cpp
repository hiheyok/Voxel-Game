#include "RenderEngine/UI/UIManager.h"

#include <cassert>

#include "RenderEngine/UI/ScreenManager.h"
#include "RenderEngine/UI/ScreenRegistry.h"
#include "RenderEngine/UI/Screens/MainScreen.h"
#include "RenderEngine/UI/UIRenderer.h"

UIManager::UIManager(GameContext& game_context)
    : game_context_{game_context},
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

#include <iostream>

void UIManager::Render() {
  assert(renderer_ && screen_manager_);
  renderer_->Clear();
  screen_manager_->SubmitToRenderer(*renderer_);
  renderer_->Render();
}