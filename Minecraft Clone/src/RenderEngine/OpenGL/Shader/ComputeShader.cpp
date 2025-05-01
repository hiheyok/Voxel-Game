#include "RenderEngine/OpenGL/Shader/ComputeShader.h"

ComputeShader::ComputeShader(std::string source) {
  std::string shaderSource = ReadFile(source);

  shader_id_ = glCreateProgram();
  GLuint shader = CompileShader(shaderSource, "Compute", GL_COMPUTE_SHADER);

  glAttachShader(shader_id_, shader);
  glLinkProgram(shader_id_);

  CheckCompileErrors(shader_id_, "PROGRAM");

  glDeleteShader(shader);  // Cleanup
}

void ComputeShader::DispatchCompute(int x, int y, int z) {
  Use();
  int maxWorkGroupCount[3];
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxWorkGroupCount[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxWorkGroupCount[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxWorkGroupCount[2]);

  if (x > maxWorkGroupCount[0] || y > maxWorkGroupCount[1] ||
      z > maxWorkGroupCount[2]) {
    g_logger.LogError("ComputeShader::DispatchCompute",
                      "Work group size exceeds hardware limits.");
    return;
  }

  glDispatchCompute(x, y, z);
}
