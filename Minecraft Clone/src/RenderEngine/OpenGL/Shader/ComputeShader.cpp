// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Shader/ComputeShader.h"

#include <stdexcept>
#include <string>

#include "Assets/Types/ShaderSource.h"

ComputeShader::ComputeShader(GameContext& context, const std::string& name,
                             AssetHandle<ShaderSource> shader_src)
    : ShaderInterface{context, name, shader_src} {}

void ComputeShader::Load() {
  const std::string& src = shader_src_->GetCompute();

  shader_id_ = glCreateProgram();
  GLuint shader = CompileShader(src, "Compute", GL_COMPUTE_SHADER);

  glAttachShader(shader_id_, shader);
  glLinkProgram(shader_id_);

  CheckCompileErrors(shader_id_, "PROGRAM");

  glDeleteShader(shader);
}

void ComputeShader::DispatchCompute(int x, int y, int z) {
  Use();
  int maxWorkGroupCount[3];
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxWorkGroupCount[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxWorkGroupCount[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxWorkGroupCount[2]);

  if (x > maxWorkGroupCount[0] || y > maxWorkGroupCount[1] ||
      z > maxWorkGroupCount[2]) {
    throw std::runtime_error(
        "ComputeShader::DispatchCompute - Work group size exceeds hardware "
        "limits.");
  }

  glDispatchCompute(x, y, z);
}
