// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/UI/Screens/DebugOverlay.h"

#include <memory>
#include <string>
#include <utility>

#include "RenderEngine/UI/Components/ColoredComponent.h"
#include "RenderEngine/UI/Widgets/TextWidget.h"
#include "RenderEngine/UI/Widgets/Widget.h"
#include "Utils/Assert.h"

using std::make_unique;
using std::move;
using std::string;
using std::to_string;

DebugOverlay::DebugOverlay(GameContext& context, ScreenManager& screen_mgr)
    : Screen{context, screen_mgr, {1920.0f, 1080.0f}} {
  root_widget_->SetAnchorBoth({0.0f, 0.0f});
  root_widget_->SetPivot({0.0f, 0.0f});
  root_widget_->SetOffsetMax(GetVirtualRes());
}

DebugOverlay::~DebugOverlay() = default;

void DebugOverlay::OnEnter() {
  glm::vec4 bg_color = {0.2f, 0.2f, 0.2f, 0.5f};

  float widget_height = 4.5f;
  float widget_width = 150.0f;

  // Title widget
  auto title = make_unique<TextWidget>(context_);
  title->SetAnchorBoth({0.0f, 1.0f});  // Top-left
  title->SetPivot({0.0f, 1.0f});
  title->SetOffsetMax({widget_width, widget_height});
  title->SetText("Version 1.3.0A");
  title->SetBackgroundColor(bg_color);
  title_widget_ = title.get();
  root_widget_->AddChildWidget(move(title));

  for (int i = 0; i < kNumStats; i++) {
    auto stat = make_unique<TextWidget>(context_);
    stat->SetAnchorBoth({0.0f, 1.0f});
    stat->SetPivot({0.0f, 1.0f});

    float y_pos = widget_height * i;
    stat->SetOffsetMin({0.0f, -y_pos - widget_height});
    stat->SetOffsetMax({widget_width, -y_pos});
    stat->SetText("N/A");
    stat->SetBackgroundColor(bg_color);

    stat_widgets_[i] = stat.get();
    root_widget_->AddChildWidget(move(stat));
  }
}

void DebugOverlay::OnPause() {}
void DebugOverlay::OnResume() {}
void DebugOverlay::OnExit() {}

void DebugOverlay::EditText(int idx, const string& text) {
  GAME_ASSERT(idx >= 0 && idx < kNumStats && stat_widgets_[idx],
              "Out of bound index");

  // Handle "Stat1" through "Stat10"
  if (idx >= 0 && idx < kNumStats && stat_widgets_[idx]) {
    stat_widgets_[idx]->SetText(text);
    stat_widgets_[idx]->SetDirty();
  }
}
