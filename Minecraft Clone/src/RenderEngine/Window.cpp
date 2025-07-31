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
  LogUtils* logger = static_cast<LogUtils*>(const_cast<void*>(userParam));
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

GLFWwindow* Window::GetWindow() { return window_; }

bool Window::WindowCloseCheck() { return glfwWindowShouldClose(window_); }

Window::Window(GameContext& game_context) : game_context_{game_context} {
  if (properties_.initialized_) {
    game_context_.logger_->LogDebug("Window::Start", "Already initialized");
    return;
  }

  glfwInit();
  if (!glfwInit()) {
    throw std::runtime_error("Window::Start - Initialization Failed: GLFW");
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window_ = glfwCreateWindow(properties_.window_size_x_,
                             properties_.window_size_y_, "1.2.0A (DEV)", 0, 0);

  glfwMakeContextCurrent(window_);

  if (window_ == nullptr) {
    glfwTerminate();
    throw std::runtime_error("Window::Start - Failed to create GLFW Window");
  } else {
    game_context_.logger_->LogInfo("Window::Start", "Created GLFW Window");
  }

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* win, int a, int b) {
    static_cast<Window*>(glfwGetWindowUserPointer(win))
        ->ResizeWindowCallback(a, b);
  });
  glfwSetCursorPosCallback(
      window_, +[](GLFWwindow* win, double a, double b) {
        static_cast<Window*>(glfwGetWindowUserPointer(win))
            ->MousePositionCallback(a, b);
      });
  glfwSetMouseButtonCallback(
      window_, +[](GLFWwindow* win, int a, int b, int c) {
        static_cast<Window*>(glfwGetWindowUserPointer(win))
            ->MouseButtonCallback(a, b);
      });
  glfwSetKeyCallback(
      window_,
      +[](GLFWwindow* win, int key, int scancode, int action, int mods) {
        static_cast<Window*>(glfwGetWindowUserPointer(win))
            ->KeyboardCallback(win, key, scancode, action, mods);
      });
  glfwSetScrollCallback(
      window_, +[](GLFWwindow* win, double a, double b) {
        static_cast<Window*>(glfwGetWindowUserPointer(win))
            ->ScrollCallback(win, a, b);
      });
  glewExperimental = GL_TRUE;
  glewInit();

  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("Window::Start - Initialization Failed: GLEW");
    return;
  }

  std::stringstream str;
  str << "OpenGL Version: " << glGetString(GL_VERSION);
  game_context_.logger_->LogInfo("Window::Start", str.str());

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(glDebugOutput,
                         static_cast<const void*>(&game_context_.logger_));
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                        GL_TRUE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glfwSwapInterval(0);
}
Window::~Window() { glfwDestroyWindow(GetWindow()); }

void Window::MousePositionCallback(double xpos, double ypos) {
  inputs_.mouse_.displacement_ =
      glm::dvec2(xpos, ypos) - inputs_.mouse_.position_;
  inputs_.mouse_.position_ = glm::dvec2(xpos, ypos);
}

void Window::UpdateWindowName(std::string name) {
  glfwSetWindowTitle(window_, name.c_str());
}

void Window::RenderLines() { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }

void Window::RenderSolid() { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

void Window::ResizeWindowCallback(int x, int y) {
  glViewport(0, 0, x, y);
  properties_.window_size_x_ = x;
  properties_.window_size_y_ = y;
  properties_.window_size_dirty_ = true;

  game_context_.logger_->LogInfo(
      "Window::ResizeWindowCallback",
      " Resized Window: " + std::to_string(x) + ", " + std::to_string(y));
}

void Window::ScrollCallback(GLFWwindow* win, double xOffset, double yOffset) {
  if (yOffset == -1.0) {
    inputs_.mouse_.scroll_direction_ = MouseInputs::ScrollState::SCROLL_DOWN;
    return;
  }

  if (yOffset == 1.0) {
    inputs_.mouse_.scroll_direction_ = MouseInputs::ScrollState::SCROLL_UP;
    return;
  }
  (void)xOffset;
  (void)win;
}

void Window::Refresh() { glfwSwapBuffers(window_); }

void Window::PollInputs() { glfwPollEvents(); }

WindowProperties& Window::GetProperties() { return properties_; }

InputManager& Window::GetUserInputs() { return inputs_; }

void Window::DisableCursor() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::EnableCursor() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::KeyboardCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods) {
  if (action == GLFW_RELEASE) {
    inputs_.ReleaseIndividualKey(key);
    return;
  }

  if ((action == GLFW_REPEAT) || (action == GLFW_PRESS)) {  // prob irrelevant
    inputs_.PressIndividualKey(key);
  }
}

void Window::MouseButtonCallback(int button, int action) {
  MouseInputs::ButtonState state = MouseInputs::ButtonState::RELEASE;
  MouseInputs::ButtonState hold_state = MouseInputs::ButtonState::HOLD;

  if (action == GLFW_PRESS) {
    state = MouseInputs::ButtonState::PRESS;
  } else {
    hold_state = state;
  }

  switch (button) {
    case GLFW_MOUSE_BUTTON_RIGHT: {
      if (inputs_.mouse_.right_ == state) {
        inputs_.mouse_.right_ = hold_state;
      } else {
        inputs_.mouse_.right_ = state;
      }
      break;
    }
    case GLFW_MOUSE_BUTTON_LEFT: {
      if (inputs_.mouse_.left_ == state) {
        inputs_.mouse_.left_ = hold_state;
      } else {
        inputs_.mouse_.left_ = state;
      }
      break;
    }
    case GLFW_MOUSE_BUTTON_MIDDLE: {
      if (inputs_.mouse_.middle_ == state) {
        inputs_.mouse_.middle_ = hold_state;
      } else {
        inputs_.mouse_.middle_ = state;
      }
      break;
    }
    default: {
      // TODO(hiheyok): Not handled yet; add more mouse button support later.
      // https://www.glfw.org/docs/3.3/group__buttons.html
    }
  }
}
