// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <vector>

#include "RenderEngine/OpenGL/Render/RenderObject.h"
#include "RenderEngine/Opengl/Buffers/Buffer.h"

class RenderDrawArrays : public RenderObject {
 public:
  // Triangles are the default draw type
  RenderDrawArrays(GameContext&, uint32_t draw_type = GL_TRIANGLES);
  ~RenderDrawArrays();

  void Render() override;

  // Used to set the rendering data
  template <typename T>
  void SetData(const std::vector<T>& data) {
    vbo_.InsertData(data, GL_STATIC_DRAW);
  }

  void SetDataAttribute(int index, int size, int type, int stride, int offset);

  void SetIndicesCount(size_t count) noexcept;
  void SetIndicesOffset(size_t offset) noexcept;

 private:
  uint32_t draw_type_ = 0;
  size_t indices_count_ = 0;
  size_t indices_offset_ = 0;
  Buffer vbo_;
};
