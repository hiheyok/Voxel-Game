// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <gl/glew.h>

#include <optional>
#include <string>
#include <variant>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>


#include "Core/Typenames.h"

class ShaderInterface {
 public:
  ShaderInterface();
  ~ShaderInterface();

  void Use();

  ShaderInterface& SetBool(const std::string& name, bool value);

  ShaderInterface& SetInt(const std::string& name, int value);

  ShaderInterface& SetFloat(const std::string& name, float value);

  ShaderInterface& SetVec2(const std::string& name, const glm::vec2& value);

  ShaderInterface& SetVec2(const std::string& name, float x, float y);

  ShaderInterface& SetVec3(const std::string& name, const glm::vec3& value);

  ShaderInterface& SetVec3(const std::string& name, float x, float y, float z);

  ShaderInterface& SetIVec3(const std::string& name, const glm::ivec3& value);

  ShaderInterface& SetIVec3(const std::string& name, int x, int y, int z);

  ShaderInterface& SetVec4(const std::string& name, const glm::vec4& value);

  ShaderInterface& SetVec4(const std::string& name, float x, float y, float z,
                           float w);

  ShaderInterface& SetMat2(const std::string& name, const glm::mat2& mat);

  ShaderInterface& SetMat3(const std::string& name, const glm::mat3& mat);

  ShaderInterface& SetMat4(const std::string& name, const glm::mat4& mat);

  ShaderInterface& BindTexture2D(GLuint index, GLuint img,
                                 const std::string& name);

  ShaderInterface& BindTextureArray2D(GLuint index, GLuint img,
                                      const std::string& name);

  ShaderInterface& BindBufferAsSSBO(GLuint buffer, int idx);
  ShaderInterface& UnbindBufferSSBO(int idx);

  ShaderInterface& SSBOMemoryBarrier();

  uint32_t shader_id_ = 0;

 protected:
  FastHashMap<std::string, int> cache_;

  // Value cache | Used to make sure to not update if the values are the same
  FastHashMap<std::string, bool> cache_bool_;
  FastHashMap<std::string, int> cache_int_;
  FastHashMap<std::string, float> cache_float_;
  FastHashMap<std::string, glm::vec2> cache_vec2_;
  FastHashMap<std::string, glm::vec3> cache_vec3_;
  FastHashMap<std::string, glm::vec4> cache_vec4_;
  FastHashMap<std::string, glm::ivec2> cache_ivec2_;
  FastHashMap<std::string, glm::ivec3> cache_ivec3_;
  FastHashMap<std::string, glm::mat2> cache_mat2_;
  FastHashMap<std::string, glm::mat3> cache_mat3_;
  FastHashMap<std::string, glm::mat4> cache_mat4_;

  std::string ReadFile(std::string path);

  GLint GetUniformLocation(std::string name);
  void CheckCompileErrors(GLuint shader, std::string type);
  GLuint CompileShader(std::string source, std::string type, GLuint shaderType);
};
