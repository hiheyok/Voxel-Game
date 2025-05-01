#include "RenderEngine/GUI/GUI.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "RenderEngine/GUI/GUIObject.h"
#include "RenderEngine/GUI/GUISet.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/Window.h"
#include "Utils/LogUtils.h"

GUI::GUI(Window* win)
    : shader_{std::make_unique<Shader>("assets/shaders/GUI/GUIVert.glsl",
                                       "assets/shaders/GUI/GUIFrag.glsl")},
      window_{win},
      guis_{} {};

GUI::GUI(GUI&&) = default;
GUI::~GUI() = default;

size_t GUI::AddGUI(std::string Name, GUISet set) {
  guis_.push_back(std::move(set));
  return guis_.size() - 1;
}

GUISet& GUI::EditGUISet(size_t GUIIndex) { return guis_[GUIIndex]; }

void GUI::PrepareRenderer() {
  for (auto& gui : guis_) {
    gui.PrepareRenderer();
  }
}

void GUI::Render() {
  Update();
  SetupDrawCalls();

  shader_->Use();
  for (auto& gui : guis_) {
    shader_->BindTexture2D(0, gui.GetGUITextureID(), "GUITexture");
    for (int i = 0; i < gui.GetNumRenderableObjects(); i++) {
      gui.vaos_[i].Bind();
      gui.ebos_[i].Bind();
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gui.vbo_size_[i]),
                     GL_UNSIGNED_INT, 0);
      gui.ebos_[i].Unbind();
      gui.vaos_[i].Unbind();
    }
  }
}

void GUI::SetupDrawCalls() {
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_CULL_FACE);
}

void GUI::Update() {
  int height, width;
  glfwGetWindowSize(window_->GetWindow(), &width, &height);

  shader_->Use();
  shader_->SetFloat("AspectRatio", static_cast<float>(height) / width);
}
