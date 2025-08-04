// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <gl/glew.h>
#include <glfw/glfw3.h>

#include <glm/vec2.hpp>
#include <string>
#include <vector>

#include "../Client/Inputs/InputManager.h"

class GameContext;

struct WindowProperties {
  int res_x_ = 1280;
  int res_y_ = 720;

  bool size_dirty_ = false;
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

  void MousePositionCallback(GLFWwindow* win, double x_pos, double y_pos);
  void MouseButtonCallback(GLFWwindow* win, int button, int action, int mods);
  void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action,
                        int mods);
  void ResizeWindowCallback(GLFWwindow* w, int x, int y);
  void ScrollCallback(GLFWwindow* win, double xoffset, double yoffset);

  GameContext& game_context_;
  InputManager inputs_;
  WindowProperties properties_;

 private:
  void InitializeGLFW();
  void InitializeCallbacks();
  void InitializeGLEW();
  void InitializeDebugCallback();
  static void APIENTRY glDebugOutput(GLenum source, GLenum type, uint32_t id,
                                     GLenum severity, GLsizei length,
                                     const char* message,
                                     const void* userParam);

  GLFWwindow* win_ = nullptr;
};
