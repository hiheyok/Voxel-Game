// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/GUI/TextRenderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/GUI/Font.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Render/RenderDrawArrays.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/OpenGL/Texture/Types/Texture2D.h"
#include "Utils/LogUtils.h"

TextRenderer::TextRenderer(GameContext& game_context)
    : game_context_{game_context},
      font_texture_{std::make_unique<Texture2D>(game_context_)},
      background_render_{std::make_unique<RenderDrawArrays>(game_context_)},
      font_render_{std::make_unique<RenderDrawArrays>(game_context_)} {}

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
  font_render_->SetShader("assets/shaders/Font/FontVert.glsl",
                          "assets/shaders/Font/FontFrag.glsl");
  background_render_->SetShader("assets/shaders/Font/FontBackgroundVert.glsl",
                                "assets/shaders/Font/FontBackgroundFrag.glsl");

  // Load textures
  RawTextureData RawTexture{"assets/minecraft/textures/font/ascii.png"};
  font_texture_->Load(RawTexture);
  font_render_->SetTexture2D(0, font_texture_->Get(), "FontTexture");
  window_ = w;
  game_context_.logger_->LogDebug("TextRenderer::InitializeTextRenderer",
                                  "Initialized font renderer");
}

void TextRenderer::InsertFontObject(const std::string& name,
                                    RenderableFont font) {
  if (font_map_.count(name)) {
    throw std::logic_error(
        "TextRenderer::InsertFontObject - " +
        std::string("Font with the name " + name + " already exist!"));
  }

  font_map_[name] = font;
}

void TextRenderer::RemoveFontObject(const std::string& name) {
  if (!font_map_.count(name)) {
    throw std::logic_error("TextRenderer::RemoveFontObject - " +
                           std::string("Font with the name " + name +
                                       " doesn't exist! Cannot remove."));
  }

  font_map_.erase(name);
}

void TextRenderer::EditFontText(const std::string& name,
                                const std::string& text) {
  font_map_[name].SetText(text);
}

void TextRenderer::ConstructBuffer() {
  // Fill text buffer
  vertices_.clear();

  for (const auto& font : font_map_) {
    std::vector<float> fontVertices = font.second.GetVertices();

    vertices_.insert(vertices_.end(), fontVertices.begin(), fontVertices.end());
  }

  font_render_->SetData(vertices_);
  font_render_->SetIndicesCount(vertices_.size() / 11);

  // Fill background buffer
  vertices_background_.clear();

  for (const auto& font : font_map_) {
    if (!font.second.background_) continue;

    std::vector<float> backgroundVertices = font.second.GetBackgroundVertices();

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

  font_render_->GetShader().SetFloat("AspectRatio",
                                     static_cast<float>(height) / width);

  background_render_->GetShader().SetFloat("AspectRatio",
                                           static_cast<float>(height) / width);
}

void TextRenderer::RenderFont() {
  Prepare();
  background_render_->Render();
  font_render_->Render();
}
