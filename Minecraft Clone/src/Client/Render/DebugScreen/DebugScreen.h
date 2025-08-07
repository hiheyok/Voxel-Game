// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <glm/vec3.hpp>
#include <memory>
#include <string>

class TextRenderer;
class Timer;
class GameContext;

struct GLFWwindow;

class DebugScreen {
 public:
  explicit DebugScreen(GameContext&);
  ~DebugScreen();
  void Render();
  void Update();
  void SetUpdateRate(int rate);
  void EditText(const std::string& name, const char* c);
  void EditText(const std::string& name, const std::string& text);
  void Initialize(GLFWwindow* w);

 private:
  GameContext& context_;
  GLFWwindow* window_{nullptr};
  std::unique_ptr<TextRenderer> renderer_;
  std::unique_ptr<Timer> timer_;
  float fps_{0.0f};
  int update_rate_ = 50;
  int vram_usage_mb_{0};
  glm::vec3 position_{0.f, 0.f, 0.f};
};
