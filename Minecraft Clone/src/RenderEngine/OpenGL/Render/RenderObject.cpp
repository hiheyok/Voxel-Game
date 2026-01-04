// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Render/RenderObject.h"

#include <string>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"

RenderObject::RenderObject(GameContext& context) noexcept
    : context_{context}, shader_{nullptr}, vao_{context} {}
RenderObject::RenderObject(RenderObject&&) noexcept = default;
RenderObject& RenderObject::operator=(RenderObject&& other) noexcept {
  if (this != &other) {
    shader_ = std::move(other.shader_);
    vao_ = std::move(other.vao_);
    texture_2d_cache_ = std::move(other.texture_2d_cache_);
  }
  return *this;
}
RenderObject::~RenderObject() noexcept = default;

void RenderObject::SetShader(const std::string& asset_key) {
  shader_ = context_.render_resource_manager_->GetShader(asset_key);
}

RenderHandle<Shader> RenderObject::GetShader() { return shader_; }

void RenderObject::SetTexture2D(int index, int id, const std::string& name) {
  shader_->BindTexture2D(index, id, name);

  // Tries to search for the the index first
  for (auto& [tex_index, tex_id, tex_name] : texture_2d_cache_) {
    if (tex_index == index) {
      tex_name = name;
      tex_id = id;
      return;
    }
  }

  texture_2d_cache_.emplace_back(index, id, name);
}

void RenderObject::SetTexture(int idx, RenderHandle<Texture> handle) {
  shader_->BindTexture(idx, handle);

  // Tries to search for the the index first
  for (auto& [tex_idx, tex_handle] : texture_cache_) {
    if (tex_idx == idx) {
      tex_handle = handle;
      return;
    }
  }

  texture_cache_.emplace_back(idx, handle);
}

void RenderObject::SetupTexture() {
  for (const auto& [index, id, name] : texture_2d_cache_) {
    shader_->BindTexture2D(index, id, name);
  }

  for (const auto& [idx, handle] : texture_cache_) {
    shader_->BindTexture(idx, handle);
  }
}

void RenderObject::EnableDepthTest() { glEnable(GL_DEPTH_TEST); }
void RenderObject::DisableDepthTest() { glDisable(GL_DEPTH_TEST); }
void RenderObject::SetDepthMask(bool mask) {
  glDepthMask(mask ? GL_TRUE : GL_FALSE);
}
void RenderObject::EnableBlend() { glEnable(GL_BLEND); }
void RenderObject::DisableBlend() { glDisable(GL_BLEND); }
void RenderObject::EnableCullFace() { glEnable(GL_CULL_FACE); }
void RenderObject::DisableCullFace() { glDisable(GL_CULL_FACE); }
void RenderObject::SetCullFace(GLenum mode) { glCullFace(mode); }