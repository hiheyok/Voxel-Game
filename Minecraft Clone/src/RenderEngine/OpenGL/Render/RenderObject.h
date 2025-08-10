// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <gl/glew.h>

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/RenderResources/RenderHandle.h"

class GameContext;
class Shader;
class TextureV2;

/*
Top abstract class for OpenGL Rendeirng
Cannot be copied
*/

class RenderObject {
 public:
  explicit RenderObject(GameContext&) noexcept;
  RenderObject(RenderObject&&) noexcept;
  RenderObject& operator=(RenderObject&&) noexcept;
  virtual ~RenderObject() noexcept;

  RenderObject(const RenderObject&) noexcept = delete;
  RenderObject& operator=(const RenderObject&) noexcept = delete;

  virtual void Render() = 0;

  void SetShader(const std::string&);

  // Use to configure shader before rendering
  RenderHandle<Shader> GetShader();
  void SetTexture2D(int index, int id, const std::string& name);

  void SetTexture(int idx, RenderHandle<TextureV2> handle);

 protected:
  static constexpr size_t GetDataTypeSize(uint32_t data_type) {
    switch (data_type) {
      case GL_BYTE:
        return 1;
      case GL_UNSIGNED_BYTE:
        return 1;
      case GL_SHORT:
        return 2;
      case GL_UNSIGNED_SHORT:
        return 2;
      case GL_INT:
        return 4;
      case GL_UNSIGNED_INT:
        return 4;
      case GL_FIXED:
        return 4;
      case GL_HALF_FLOAT:
        return 2;
      case GL_FLOAT:
        return 4;
      case GL_DOUBLE:
        return 8;
      default:
        throw std::runtime_error("Invalid datatype");
    }
  }

  template <typename T>
  static constexpr GLenum GetDataType() {
    if constexpr (std::is_same_v<T, double>) {
      return GL_DOUBLE;
    } else if constexpr (std::is_same_v<T, float>) {
      return GL_FLOAT;
    } else if constexpr (std::is_same_v<T, uint32_t>) {
      return GL_UNSIGNED_INT;
    } else if constexpr (std::is_same_v<T, uint16_t>) {
      return GL_UNSIGNED_SHORT;
    } else if constexpr (std::is_same_v<T, uint8_t>) {
      return GL_UNSIGNED_BYTE;
    } else if constexpr (std::is_same_v<T, int>) {
      return GL_INT;
    } else if constexpr (std::is_same_v<T, int16_t>) {
      return GL_SHORT;
    } else if constexpr (std::is_same_v<T, int8_t>) {
      return GL_BYTE;
    }
  }

 protected:
  void SetupTexture();

  GameContext& context_;
  RenderHandle<Shader> shader_;
  VertexArray vao_;

 private:
  // Texture cache states
  std::vector<std::tuple<int, int, std::string>>
      texture_2d_cache_;  // <index, id, name>

  std::vector<std::pair<int, RenderHandle<TextureV2>>> texture_cache_;
};
