// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Shader/Shader.h"

#include <sstream>
#include <string>

#include "Utils/LogUtils.h"

Shader::Shader() = default;

Shader::Shader(const char* vertexPath, const char* fragmentPath,
               const char* geometryPath) {
  // 1. retrieve the vertex/fragment source code from filePath
  std::string vertexCode = ReadFile(vertexPath);
  std::string fragmentCode = ReadFile(fragmentPath);
  std::string geometryCode;

  if (geometryPath != nullptr) {
    geometryCode = ReadFile(geometryPath);
  }

  g_logger.LogDebug("Shader::Init",
                    "Compiling shader: " + std::string(vertexPath) + ", " +
                        std::string(fragmentPath));

  uint32_t vertex = CompileShader(vertexCode, "Vertex", GL_VERTEX_SHADER);
  uint32_t fragment =
      CompileShader(fragmentCode, "Fragment", GL_FRAGMENT_SHADER);

  // if geometry shader is given, compile geometry shader
  uint32_t geometry;
  if (geometryPath != nullptr) {
    geometry = CompileShader(geometryCode, "Geometry", GL_GEOMETRY_SHADER);
  }

  shader_id_ = glCreateProgram();
  glAttachShader(shader_id_, vertex);
  glAttachShader(shader_id_, fragment);

  if (geometryPath != nullptr) {
    glAttachShader(shader_id_, geometry);
  }

  glLinkProgram(shader_id_);
  CheckCompileErrors(shader_id_, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (geometryPath != nullptr) {
    glDeleteShader(geometry);
  }
}
