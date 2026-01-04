// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Shader/Shader.h"

#include <cstdint>
#include <string>

#include "Assets/AssetHandle.h"
#include "Assets/Types/ShaderSource.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/OpenGL/Shader/ShaderInterface.h"

using std::string;

Shader::Shader(GameContext& context, const string& name,
               AssetHandle<ShaderSource> shader_src)
    : ShaderInterface{context, name, shader_src} {}

void Shader::Load() {
  const string& vertexCode = shader_src_->GetVertex();
  const string& fragmentCode = shader_src_->GetFragment();
  const string& geometryCode = shader_src_->GetGeometry();

  uint32_t vertex = CompileShader(vertexCode, "Vertex", GL_VERTEX_SHADER);
  uint32_t fragment =
      CompileShader(fragmentCode, "Fragment", GL_FRAGMENT_SHADER);

  // if geometry shader is given, compile geometry shader
  uint32_t geometry;
  if (geometryCode.size() != 0) {
    geometry = CompileShader(geometryCode, "Geometry", GL_GEOMETRY_SHADER);
  }

  shader_id_ = glCreateProgram();
  glAttachShader(shader_id_, vertex);
  glAttachShader(shader_id_, fragment);

  if (geometryCode.size() != 0) {
    glAttachShader(shader_id_, geometry);
  }

  glLinkProgram(shader_id_);
  CheckCompileErrors(shader_id_, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (geometryCode.size() != 0) {
    glDeleteShader(geometry);
  }
}