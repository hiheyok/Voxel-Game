// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/GUI/GUISet.h"

#include <memory>
#include <stdexcept>
#include <string>

#include "RenderEngine/GUI/GUIObject.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/OpenGL/Texture/Types/Texture2D.h"

GUISet::GUISet(GameContext& game_context)
    : game_context_{game_context},
      gui_texture_{std::make_unique<Texture2D>(game_context)} {}
GUISet::~GUISet() = default;
GUISet::GUISet(GUISet&& other) = default;

void GUISet::AddGUIElementNorm(std::string Name, std::string Text,
                               glm::vec2 Size, glm::vec2 Position,
                               glm::vec2 UV_P0, glm::vec2 UV_P1) {
  if (gui_element_index_.find(Name) == gui_element_index_.end()) {
    elements_.emplace_back(Text, Size, Position);
    elements_.back().buffer_index_ = AddRenderingObj();
    elements_.back().uv_p0_ = UV_P0;
    elements_.back().uv_p1_ = UV_P1;
    gui_element_index_.emplace(Name, static_cast<int>(elements_.size() - 1));
    num_of_renderable_objects_++;
    is_dirty_ = true;
  } else {
    throw std::logic_error("GUISet::AddGUIElementNorm - Element " + Name +
                           " already exist!");
  }
}

void GUISet::AddGUIElement(std::string name, std::string text, glm::vec2 size,
                           glm::vec2 position, glm::vec2 UV_P0,
                           glm::vec2 UV_P1) {
  UV_P0.x = UV_P0.x / gui_texture_->GetWidth();
  UV_P1.x = UV_P1.x / gui_texture_->GetWidth();

  UV_P0.y = UV_P0.y / gui_texture_->GetHeight();
  UV_P1.y = UV_P1.y / gui_texture_->GetHeight();

  if (gui_element_index_.find(name) == gui_element_index_.end()) {
    elements_.emplace_back(text, size, position);
    elements_.back().buffer_index_ = AddRenderingObj();
    elements_.back().uv_p0_ = UV_P0;
    elements_.back().uv_p1_ = UV_P1;
    gui_element_index_.emplace(name, static_cast<int>(elements_.size() - 1));
    num_of_renderable_objects_++;
    is_dirty_ = true;
  } else {
    throw std::logic_error("GUISet::AddGUIElement - Element " + name +
                           " already exist!");
  }
}

void GUISet::EditElementPosition(std::string Name, glm::vec2 Position) {
  auto it = gui_element_index_.find(Name);

  if (it != gui_element_index_.end()) {
    int idx = it->second;
    if (elements_[idx].location_ != Position) {
      elements_[idx].location_ = Position;
      is_dirty_ = true;
    }
  } else {
    throw std::logic_error("GUISet::EditElementPosition - Element " + Name +
                           " doesn't exist!");
  }
}

void GUISet::EditElementUVNorm(std::string Name, glm::vec2 UV0, glm::vec2 UV1) {
  auto it = gui_element_index_.find(Name);

  if (it != gui_element_index_.end()) {
    int idx = it->second;
    if (elements_[idx].uv_p0_ != UV0 || elements_[idx].uv_p1_ != UV1) {
      elements_[idx].uv_p0_ = UV0;
      elements_[idx].uv_p1_ = UV1;
      is_dirty_ = true;
    }
  } else {
    throw std::logic_error("GUISet::EditElementUVNorm - Element " + Name +
                           " doesn't exist!");
  }
}

void GUISet::SetGUITexture(std::string file) {
  gui_texture_ =
      std::make_unique<Texture2D>(game_context_, RawTextureData(file));
}

void GUISet::SetGUITexture(GLuint textureId, size_t x, size_t y) {
  gui_texture_->Set(textureId, y, x);
}

void GUISet::PrepareRenderer() {
  if (!is_dirty_) {
    return;
  }
  is_dirty_ = false;

  for (auto& e : elements_) {
    GUIElement::GUIVertices GUIData = e.GetVertices();
    size_t BufferIndex = e.buffer_index_;
    vbos_[BufferIndex].InsertData(GUIData.vertices_, GL_STATIC_DRAW);
    ebos_[BufferIndex].InsertData(GUIData.indices_, GL_STATIC_DRAW);
    vbo_size_[BufferIndex] = GUIData.indices_.size();
  }
}
size_t GUISet::GetNumRenderableObjects() const {
  return num_of_renderable_objects_;
}
GLuint GUISet::GetGUITextureID() const { return gui_texture_->Get(); }

size_t GUISet::AddRenderingObj() {
  vaos_.emplace_back(game_context_);
  ebos_.emplace_back(game_context_);
  vbos_.emplace_back(game_context_);

  vbos_.back().SetType(GL_ARRAY_BUFFER);
  ebos_.back().SetType(GL_ELEMENT_ARRAY_BUFFER);

  vbos_.back().SetUsage(GL_STATIC_DRAW);
  ebos_.back().SetUsage(GL_STATIC_DRAW);

  vaos_.back()
      .EnableAttriPtr(&vbos_.back(), 0, 2, GL_FLOAT, GL_FALSE, 4, 0)
      .EnableAttriPtr(&vbos_.back(), 1, 2, GL_FLOAT, GL_FALSE, 4, 2);

  vbo_size_.emplace_back(0);
  return vaos_.size() - 1;
}
