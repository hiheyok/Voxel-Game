// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <string>
#include <vector>

class GUISet;
class Shader;
class Window;
class GameContext;

struct GLFWwindow;

class GUI {
 public:
  explicit GUI(GameContext&, Window* win);
  GUI(const GUI&) = delete;
  GUI(GUI&&);
  ~GUI();

  size_t AddGUI(std::string Name, GUISet set);
  GUISet& EditGUISet(size_t GUIIndex);
  void PrepareRenderer();
  void Render();

 private:
  void SetupDrawCalls();
  void Update();

  GameContext& game_context_;
  Window* window_;
  std::vector<GUISet> guis_;
  std::unique_ptr<Shader> shader_;
};
