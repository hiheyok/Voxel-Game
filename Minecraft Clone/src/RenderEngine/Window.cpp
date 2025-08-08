// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/Window.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

void APIENTRY Window::glDebugOutput(GLenum source, GLenum type, uint32_t id,
                                    GLenum severity, GLsizei length,
                                    const char* message,
                                    const void* userParam) {
  Window* window = static_cast<Window*>(const_cast<void*>(userParam));
  LogUtils* logger = window->context_.logger_.get();

  std::stringstream str;

  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

  str << " (OPENGL) Debug message (" << id << "): " << message << " | ";

  switch (source) {
    case GL_DEBUG_SOURCE_API:
      str << "Source: API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      str << "Source: Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      str << "Source: Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      str << "Source: Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      str << "Source: Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      str << "Source: Other";
      break;
  }
  str << " | ";

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      str << "Type: Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      str << "Type: Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      str << "Type: Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      str << "Type: Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      str << "Type: Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      str << "Type: Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      str << "Type: Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      str << "Type: Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      str << "Type: Other";
      break;
  }
  str << " | ";

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      str << "Severity: high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      str << "Severity: medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      str << "Severity: low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      str << "Severity: notification";
      break;
  }

  str.seekg(0, std::ios::end);

  if (str.str().size() != 0) {
    logger->LogDebug("Window::glDebugOutput", str.str());
  }
}

template <typename T, auto MemberFunc>
auto MakeCallback() {
  return [](GLFWwindow* win, auto... args) {
    T* self = static_cast<T*>(glfwGetWindowUserPointer(win));

    if (self) {
      (self->*MemberFunc)(win, std::forward<decltype(args)>(args)...);
    }
  };
}

GLFWwindow* Window::GetWindow() { return win_; }

bool Window::WindowCloseCheck() { return glfwWindowShouldClose(win_); }

Window::Window(GameContext& context) : context_{context} {
  InitializeGLFW();
  InitializeCallbacks();
  InitializeGLEW();
  InitializeDebugCallback();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glfwSwapInterval(0);
}
Window::~Window() { glfwDestroyWindow(GetWindow()); }

void Window::MousePositionCallback(GLFWwindow* win, double x_pos,
                                   double y_pos) {
  inputs_.UpdateMouse(static_cast<float>(x_pos), static_cast<float>(y_pos));
}

void Window::UpdateWindowName(std::string name) {
  glfwSetWindowTitle(win_, name.c_str());
}

void Window::RenderLines() { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }

void Window::RenderSolid() { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

void Window::ResizeWindowCallback(GLFWwindow* win, int x, int y) {
  glViewport(0, 0, x, y);
  properties_.res_x_ = x;
  properties_.res_y_ = y;
  properties_.size_dirty_ = true;
  inputs_.UpdateResolution(x, y);

  LOG_INFO("Resized Window: {}, {}", x, y);
}

void Window::ScrollCallback(GLFWwindow* win, double x_offset, double y_offset) {
  inputs_.UpdateScroll(x_offset, y_offset);
  (void)win;
}

void Window::Refresh() { glfwSwapBuffers(win_); }

void Window::PollInputs() { glfwPollEvents(); }

WindowProperties& Window::GetProperties() { return properties_; }

InputManager& Window::GetUserInputs() { return inputs_; }

void Window::DisableCursor() {
  inputs_.SetCursor(false);
  glfwSetInputMode(win_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::EnableCursor() {
  inputs_.SetCursor(true);
  glfwSetInputMode(win_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::KeyboardCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods) {
  inputs_.UpdateKeys(key, scancode, action, mods);
}

void Window::MouseButtonCallback(GLFWwindow* win, int button, int action,
                                 int mods) {
  inputs_.UpdateButton(button, action);
}

void Window::InitializeGLFW() {
  if (properties_.initialized_) {
    LOG_DEBUG("Already initialized");
    return;
  }

  glfwInit();
  if (!glfwInit()) {
    LOG_ERROR("Initialization Failed: GLFW");
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  win_ = glfwCreateWindow(properties_.res_x_, properties_.res_y_,
                          "1.3.0A (DEV)", 0, 0);

  inputs_.UpdateResolution(properties_.res_x_, properties_.res_y_);
  inputs_.SetCursor(true);

  glfwMakeContextCurrent(win_);

  if (win_ == nullptr) {
    glfwTerminate();
    LOG_ERROR("Failed to create GLFW Window");
  } else {
    LOG_INFO("Created GLFW Window");
  }
}

void Window::InitializeCallbacks() {
  glfwSetWindowUserPointer(win_, this);
  glfwSetFramebufferSizeCallback(
      win_, MakeCallback<Window, &Window::ResizeWindowCallback>());
  glfwSetCursorPosCallback(
      win_, MakeCallback<Window, &Window::MousePositionCallback>());
  glfwSetMouseButtonCallback(
      win_, MakeCallback<Window, &Window::MouseButtonCallback>());
  glfwSetKeyCallback(win_, MakeCallback<Window, &Window::KeyboardCallback>());
  glfwSetScrollCallback(win_, MakeCallback<Window, &Window::ScrollCallback>());
}

void Window::InitializeGLEW() {
  glewInit();

  if (glewInit() != GLEW_OK) {
    LOG_ERROR("Initialization Failed: GLEW");
    return;
  }
  glewExperimental = GL_TRUE;

  std::stringstream str;
  str << "OpenGL Version: " << glGetString(GL_VERSION);
  LOG_INFO("{}",str.str());
}

void Window::InitializeDebugCallback() {
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(glDebugOutput, static_cast<const void*>(this));
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                        GL_TRUE);
}
