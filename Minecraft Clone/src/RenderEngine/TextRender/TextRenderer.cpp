// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/TextRender/TextRenderer.h"

#include <gl/glew.h>
#include <glfw/glfw3.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Assets/AssetManager.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Render/RenderDrawArrays.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2D.h"
#include "RenderEngine/TextRender/Font.h"
#include "Utils/LogUtils.h"

using std::make_unique;
using std::string;
using std::vector;

TextRenderer::TextRenderer(GameContext& context)
    : context_{context},
      background_render_{make_unique<RenderDrawArrays>(context_)},
      font_render_{make_unique<RenderDrawArrays>(context_)} {}

TextRenderer::~TextRenderer() = default;

void TextRenderer::InitializeTextRenderer(GLFWwindow* w) {
  // Setup buffer for text rendering
  font_render_->SetDataAttribute(0, 2, GL_FLOAT, 11, 0);
  font_render_->SetDataAttribute(1, 2, GL_FLOAT, 11, 2);
  font_render_->SetDataAttribute(2, 2, GL_FLOAT, 11, 4);
  font_render_->SetDataAttribute(3, 2, GL_FLOAT, 11, 6);
  font_render_->SetDataAttribute(4, 3, GL_FLOAT, 11, 8);

  // Setup buffer for background rendering
  background_render_->SetDataAttribute(0, 2, GL_FLOAT, 8, 0);
  background_render_->SetDataAttribute(1, 2, GL_FLOAT, 8, 2);
  background_render_->SetDataAttribute(2, 4, GL_FLOAT, 8, 4);

  // Initialize shaders
  font_render_->SetShader("font_render");
  background_render_->SetShader("font_background_render");

  // Load textures
  RenderHandle<Texture2D> tex_handle =
      context_.render_resource_manager_->GetTexture2D("ascii_font");
  font_render_->SetTexture(0, tex_handle);
  window_ = w;
  LOG_DEBUG("Initialized font renderer");
}

void TextRenderer::InsertFontObject(const string& name, RenderableFont font) {
  if (font_map_.count(name)) {
    throw std::logic_error(FUNC_SIGNATURE + string("Font with the name " +
                                                   name + " already exist!"));
  }

  font_map_[name] = font;
}

void TextRenderer::RemoveFontObject(const string& name) {
  if (!font_map_.count(name)) {
    throw std::logic_error("TextRenderer::RemoveFontObject - " +
                           string("Font with the name " + name +
                                  " doesn't exist! Cannot remove."));
  }

  font_map_.erase(name);
}

void TextRenderer::EditFontText(const string& name, const string& text) {
  font_map_[name].SetText(text);
}

void TextRenderer::ConstructBuffer() {
  // Fill text buffer
  vertices_.clear();

  for (const auto& font : font_map_) {
    vector<float> fontVertices = font.second.GetVertices();

    vertices_.insert(vertices_.end(), fontVertices.begin(), fontVertices.end());
  }

  font_render_->SetData(vertices_);
  font_render_->SetIndicesCount(vertices_.size() / 11);

  // Fill background buffer
  vertices_background_.clear();

  for (const auto& font : font_map_) {
    if (!font.second.background_) continue;

    vector<float> backgroundVertices = font.second.GetBackgroundVertices();

    vertices_background_.insert(vertices_background_.end(),
                                backgroundVertices.begin(),
                                backgroundVertices.end());
  }

  background_render_->SetData(vertices_background_);
  background_render_->SetIndicesCount(vertices_background_.size() / 8);
}

void TextRenderer::Prepare() {
  int height, width;
  glfwGetWindowSize(window_, &width, &height);

  font_render_->GetShader()->SetFloat("AspectRatio",
                                      static_cast<float>(height) / width);

  background_render_->GetShader()->SetFloat("AspectRatio",
                                            static_cast<float>(height) / width);
}

void TextRenderer::RenderFont() {
  // Setup GL state for text rendering
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  Prepare();
  background_render_->Render();
  font_render_->Render();

  // Restore GL state
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
}
