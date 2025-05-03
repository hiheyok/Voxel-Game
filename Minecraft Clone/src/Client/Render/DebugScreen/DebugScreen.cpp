// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Render/DebugScreen/DebugScreen.h"

#include <gl/glew.h>
#include <string>

#include "RenderEngine/GUI/Font.h"
#include "RenderEngine/GUI/TextRenderer.h"
#include "Utils/Timer/Timer.h"

DebugScreen::DebugScreen()
    : renderer_{std::make_unique<TextRenderer>()},
      timer_{std::make_unique<Timer>()} {}

DebugScreen::~DebugScreen() = default;

void DebugScreen::Render() { renderer_->RenderFont(); }

void DebugScreen::Update() {
  if (timer_->GetTimePassed_ms() > update_rate_) {
    renderer_->ConstructBuffer();
    timer_->Set();
  }
}

void DebugScreen::SetUpdateRate(int rate) { update_rate_ = rate; }

void DebugScreen::EditText(const std::string& name, const char* c) {
  EditText(name, std::string(c));
}

void DebugScreen::EditText(const std::string& name, const std::string& text) {
  renderer_->EditFontText(name, text);
}

void DebugScreen::Initialize(GLFWwindow* w) {
  window_ = w;
  renderer_->InitializeTextRenderer(w);
  glm::vec3 clr = glm::vec3(0.2f, 0.2f, 0.2f);
  float a = 0.5;

  RenderableFont versionFont;
  versionFont.FontSize(0.05f);
  versionFont.SetText(std::string("Version 1.3.0A"));
  versionFont.SetPosition(glm::vec2(-1.f, 0.95f));
  versionFont.AddBackground(clr, a);

  for (int i = 1; i <= 10; i++) {
    RenderableFont stat;
    stat.FontSize(0.04f);
    stat.SetText(std::string("N/A"));
    stat.SetPosition(glm::vec2(-1.f, 0.95f - 0.05f * i));
    stat.AddBackground(clr, a);
    stat.SetBackgroundPadding(0.005f, 0.0f);
    renderer_->InsertFontObject("Stat" + std::to_string(i), stat);
  }

  renderer_->InsertFontObject("Title", versionFont);

  Update();
}
