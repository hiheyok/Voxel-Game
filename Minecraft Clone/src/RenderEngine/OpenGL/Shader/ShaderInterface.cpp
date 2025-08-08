// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Shader/ShaderInterface.h"

#include <fstream>
#include <iostream>
#include <string>

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

ShaderInterface::ShaderInterface(GameContext& context) : context_{context} {}

ShaderInterface::~ShaderInterface() {
  if (shader_id_) {
    glDeleteProgram(shader_id_);
  }
}

ShaderInterface::ShaderInterface(ShaderInterface&& other) noexcept
    : context_{other.context_} {
  shader_id_ = other.shader_id_;
  other.shader_id_ = 0;
  cache_ = std::move(other.cache_);
  cache_bool_ = std::move(other.cache_bool_);
  cache_int_ = std::move(other.cache_int_);
  cache_float_ = std::move(other.cache_float_);
  cache_vec2_ = std::move(other.cache_vec2_);
  cache_vec3_ = std::move(other.cache_vec3_);
  cache_vec4_ = std::move(other.cache_vec4_);
  cache_ivec2_ = std::move(other.cache_ivec2_);
  cache_ivec3_ = std::move(other.cache_ivec3_);
  cache_mat2_ = std::move(other.cache_mat2_);
  cache_mat3_ = std::move(other.cache_mat3_);
  cache_mat4_ = std::move(other.cache_mat4_);
}

ShaderInterface& ShaderInterface::operator=(ShaderInterface&& other) noexcept {
  if (this != &other) {
    shader_id_ = other.shader_id_;
    other.shader_id_ = 0;
    cache_ = std::move(other.cache_);
    cache_bool_ = std::move(other.cache_bool_);
    cache_int_ = std::move(other.cache_int_);
    cache_float_ = std::move(other.cache_float_);
    cache_vec2_ = std::move(other.cache_vec2_);
    cache_vec3_ = std::move(other.cache_vec3_);
    cache_vec4_ = std::move(other.cache_vec4_);
    cache_ivec2_ = std::move(other.cache_ivec2_);
    cache_ivec3_ = std::move(other.cache_ivec3_);
    cache_mat2_ = std::move(other.cache_mat2_);
    cache_mat3_ = std::move(other.cache_mat3_);
    cache_mat4_ = std::move(other.cache_mat4_);
  }
  return *this;
}

void ShaderInterface::Use() { glUseProgram(shader_id_); }

ShaderInterface& ShaderInterface::SetBool(const std::string& name, bool value) {
  auto it = cache_bool_.find(name);
  if (it != cache_bool_.end()) {
    if (it->second == value) return *this;
    it->second = value;
  } else {
    cache_bool_[name] = value;
  }
  Use();
  glUniform1i(GetUniformLocation(name), static_cast<int>(value));
  return *this;
}

ShaderInterface& ShaderInterface::SetInt(const std::string& name, int value) {
  auto it = cache_int_.find(name);
  if (it != cache_int_.end()) {
    if (it->second == value) return *this;
    it->second = value;
  } else {
    cache_int_[name] = value;
  }
  Use();
  glUniform1i(GetUniformLocation(name), value);
  return *this;
}

ShaderInterface& ShaderInterface::SetFloat(const std::string& name,
                                           float value) {
  auto it = cache_float_.find(name);
  if (it != cache_float_.end()) {
    if (it->second == value) return *this;
    it->second = value;
  } else {
    cache_float_[name] = value;
  }
  Use();
  glUniform1f(GetUniformLocation(name), value);
  return *this;
}

ShaderInterface& ShaderInterface::SetVec2(const std::string& name,
                                          const glm::vec2& value) {
  auto it = cache_vec2_.find(name);
  if (it != cache_vec2_.end()) {
    if (it->second == value) return *this;
    it->second = value;
  } else {
    cache_vec2_[name] = value;
  }
  Use();
  glUniform2fv(GetUniformLocation(name), 1, &value[0]);
  return *this;
}

ShaderInterface& ShaderInterface::SetVec2(const std::string& name, float x,
                                          float y) {
  auto it = cache_vec2_.find(name);
  if (it != cache_vec2_.end()) {
    if (it->second == glm::vec2(x, y)) return *this;
    it->second = {x, y};
  } else {
    cache_vec2_[name] = {x, y};
  }
  Use();
  glUniform2f(GetUniformLocation(name), x, y);
  return *this;
}

ShaderInterface& ShaderInterface::SetVec3(const std::string& name,
                                          const glm::vec3& value) {
  auto it = cache_vec3_.find(name);
  if (it != cache_vec3_.end()) {
    if (it->second == value) return *this;
    it->second = value;
  } else {
    cache_vec3_[name] = value;
  }
  Use();
  glUniform3fv(GetUniformLocation(name), 1, &value[0]);
  return *this;
}

ShaderInterface& ShaderInterface::SetVec3(const std::string& name, float x,
                                          float y, float z) {
  auto it = cache_vec3_.find(name);
  if (it != cache_vec3_.end()) {
    if (it->second == glm::vec3(x, y, z)) return *this;
    it->second = {x, y, z};
  } else {
    cache_vec3_[name] = {x, y, z};
  }
  Use();
  glUniform3f(GetUniformLocation(name), x, y, z);
  return *this;
}

ShaderInterface& ShaderInterface::SetIVec3(const std::string& name,
                                           const glm::ivec3& value) {
  auto it = cache_ivec3_.find(name);
  if (it != cache_ivec3_.end()) {
    if (it->second == value) return *this;
    it->second = value;
  } else {
    cache_ivec3_[name] = value;
  }
  Use();
  glUniform3iv(GetUniformLocation(name), 1, &value[0]);
  return *this;
}

ShaderInterface& ShaderInterface::SetIVec3(const std::string& name, int x,
                                           int y, int z) {
  auto it = cache_ivec3_.find(name);
  if (it != cache_ivec3_.end()) {
    if (it->second == glm::ivec3(x, y, z)) return *this;
    it->second = {x, y, z};
  } else {
    cache_ivec3_[name] = {x, y, z};
  }
  Use();
  glUniform3i(GetUniformLocation(name), x, y, z);
  return *this;
}

ShaderInterface& ShaderInterface::SetVec4(const std::string& name,
                                          const glm::vec4& value) {
  auto it = cache_vec4_.find(name);
  if (it != cache_vec4_.end()) {
    if (it->second == value) return *this;
    it->second = value;
  } else {
    cache_vec4_[name] = value;
  }
  Use();
  glUniform4fv(GetUniformLocation(name), 1, &value[0]);
  return *this;
}

ShaderInterface& ShaderInterface::SetVec4(const std::string& name, float x,
                                          float y, float z, float w) {
  auto it = cache_vec4_.find(name);
  if (it != cache_vec4_.end()) {
    if (it->second == glm::vec4(x, y, z, w)) return *this;
    it->second = {x, y, z, w};
  } else {
    cache_vec4_[name] = {x, y, z, w};
  }
  Use();
  glUniform4f(GetUniformLocation(name), x, y, z, w);
  return *this;
}

ShaderInterface& ShaderInterface::SetMat2(const std::string& name,
                                          const glm::mat2& mat) {
  auto it = cache_mat2_.find(name);
  if (it != cache_mat2_.end()) {
    if (it->second == mat) return *this;
    it->second = mat;
  } else {
    cache_mat2_[name] = mat;
  }
  Use();
  glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
  return *this;
}

ShaderInterface& ShaderInterface::SetMat3(const std::string& name,
                                          const glm::mat3& mat) {
  auto it = cache_mat3_.find(name);
  if (it != cache_mat3_.end()) {
    if (it->second == mat) return *this;
    it->second = mat;
  } else {
    cache_mat3_[name] = mat;
  }
  Use();
  glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
  return *this;
}

ShaderInterface& ShaderInterface::SetMat4(const std::string& name,
                                          const glm::mat4& mat) {
  auto it = cache_mat4_.find(name);
  if (it != cache_mat4_.end()) {
    if (it->second == mat) return *this;
    it->second = mat;
  } else {
    cache_mat4_[name] = mat;
  }
  Use();
  glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
  return *this;
}

ShaderInterface& ShaderInterface::BindTexture2D(GLuint index, GLuint img,
                                                const std::string& name) {
  SetInt(name, index);
  Use();
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_2D, img);
  return *this;
}

ShaderInterface& ShaderInterface::BindTextureArray2D(GLuint index, GLuint img,
                                                     const std::string& name) {
  SetInt(name, index);
  Use();
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_2D_ARRAY, img);
  return *this;
}

GLint ShaderInterface::GetUniformLocation(std::string name) {
  if (cache_.count(name)) {
    return cache_[name];
  } else {
    GLint location = glGetUniformLocation(shader_id_, name.c_str());
    cache_[name] = location;
    return location;
  }
}

GLuint ShaderInterface::CompileShader(std::string source, std::string type,
                                      GLuint shaderType) {
  GLuint shader = glCreateShader(shaderType);
  const char* src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  // Check for errors
  CheckCompileErrors(shader, type);
  return shader;
}

void ShaderInterface::CheckCompileErrors(GLuint shader, std::string type) {
  GLint success;
  GLchar log[2048];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 2048, 0, log);
      LOG_ERROR("Failed to compile {} Shader: \n {} \n", type,
                std::string(log));
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, 0, log);
      LOG_ERROR("Failed to link Shader Program: \n {} \n", std::string(log));
    }
  }
}

std::string ShaderInterface::ReadFile(std::string path) {
  std::ifstream file;
  std::string code;

  try {
    file.open(path);
    std::stringstream shaderStream;
    shaderStream << file.rdbuf();
    file.close();
    code = shaderStream.str();
  } catch (std::ifstream::failure& e) {
    LOG_ERROR("Failed to read file: {}, {}", path, e.what());
  }

  return code;
}

ShaderInterface& ShaderInterface::BindBufferAsSSBO(GLuint buffer, int idx) {
  Use();
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, idx, buffer);
  return *this;
}

ShaderInterface& ShaderInterface::UnbindBufferSSBO(int idx) {
  Use();
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, idx, 0);
  return *this;
}

ShaderInterface& ShaderInterface::SSBOMemoryBarrier() {
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  return *this;
}
