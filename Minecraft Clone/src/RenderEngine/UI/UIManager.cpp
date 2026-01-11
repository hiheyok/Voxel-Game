#include "RenderEngine/UI/UIManager.h"

#include <cassert>
#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "Client/Inputs/InputEvent.h"
#include "Client/Inputs/InputManager.h"
#include "RenderEngine/UI/ScreenManager.h"
#include "RenderEngine/UI/ScreenRegistry.h"
#include "RenderEngine/UI/Screens/DebugOverlay.h"
#include "RenderEngine/UI/Screens/MainScreen.h"
#include "RenderEngine/UI/Screens/PlayerHud.h"
#include "RenderEngine/UI/UIRenderer.h"
#include "Utils/Assert.h"

using std::make_unique;
using std::move;
using std::vector;

UIManager::UIManager(GameContext& context, InputManager& input_mgr)
    : context_{context},
      input_mgr_{input_mgr},
      renderer_{make_unique<UIRenderer>(context_)},
      screen_registry_{make_unique<ScreenRegistry>(context_)},
      screen_manager_{make_unique<ScreenManager>(context_, *screen_registry_)} {
}

UIManager::~UIManager() = default;

void UIManager::Initialize() {
  GAME_ASSERT(screen_registry_ != nullptr,
              "Screen Registry hasn't been created yet");
  screen_registry_->RegisterScreen<MainScreen>("debug_screen");
  screen_registry_->RegisterScreen<PlayerHud>("player_hud");
  screen_registry_->RegisterScreen<DebugOverlay>("debug_overlay");
}

void UIManager::PopScreen() { screen_manager_->PopScreen(); }

void UIManager::Render() {
  GAME_ASSERT(renderer_ && screen_manager_,
              "Renderer or screen manager is null");
  renderer_->Clear();
  screen_manager_->SubmitToRenderer(*renderer_);
  renderer_->Render();
}

void UIManager::Update() { screen_manager_->Update(input_mgr_.GetEvents()); }

void UIManager::ScreenResChanged(glm::vec2 res) {
  if (curr_res_ != res) {
    curr_res_ = res;
    screen_manager_->ChangeVirtualRes(res);
    renderer_->SetScreenRes(res);
  }
}

void UIManager::SetScreenTickCallback(int idx, Screen::TickCallback callback) {
  screen_manager_->SetScreenTickCallback(idx, move(callback));
}
