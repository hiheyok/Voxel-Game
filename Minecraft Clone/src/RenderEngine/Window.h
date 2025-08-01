// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "../Client/Inputs/InputManager.h"

class GameContext;

struct WindowProperties {
  int window_size_x_ = 1280;
  int window_size_y_ = 720;

  bool window_size_dirty_ = false;
  bool initialized_ = false;
  bool draw_solid_ = true;
};

class Window {
 public:
  Window(GameContext&);
  ~Window();

  void Refresh();
  void PollInputs();
  WindowProperties& GetProperties();
  InputManager& GetUserInputs();
  GLFWwindow* GetWindow();

  void RenderSolid();
  void RenderLines();

 protected:
  bool WindowCloseCheck();
  void DisableCursor();
  void EnableCursor();

  void UpdateWindowName(std::string name);

  void MousePositionCallback(double xpos, double ypos);
  void MouseButtonCallback(int button, int action);
  void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action,
                        int mods);
  void ResizeWindowCallback(int x, int y);
  void ScrollCallback(GLFWwindow* win, double xoffset, double yoffset);
  
  GameContext& game_context_;
  InputManager inputs_;
  WindowProperties properties_;

 private:
  static void APIENTRY glDebugOutput(GLenum source, GLenum type, uint32_t id,
                                     GLenum severity, GLsizei length,
                                     const char* message,
                                     const void* userParam);

  GLFWwindow* window_ = nullptr;
};
