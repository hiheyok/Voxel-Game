// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Render/RenderObject.h"

#include <utility>

RenderObject::RenderObject() noexcept = default;
RenderObject::RenderObject(RenderObject&&) noexcept = default;
RenderObject& RenderObject::operator=(RenderObject&&) noexcept = default;
RenderObject::~RenderObject() noexcept = default;

void RenderObject::SetShader(std::unique_ptr<ShaderInterface> shader) {
  shader_ = std::move(shader);
}

ShaderInterface& RenderObject::GetShader() { return *shader_; }
