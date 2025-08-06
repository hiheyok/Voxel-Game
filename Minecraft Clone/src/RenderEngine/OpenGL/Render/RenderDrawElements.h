// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <vector>

#include "RenderEngine/OpenGL/Render/RenderObject.h"
#include "RenderEngine/Opengl/Buffers/Buffer.h"

class RenderDrawElements : public RenderObject {
 public:
  // Triangles are the default draw type
  RenderDrawElements(GameContext&, uint32_t draw_type = GL_TRIANGLES);
  ~RenderDrawElements();

  void Render() override;

  // Used to set the rendering data
  template <typename VertexType, typename ElementType>
  void SetData(const std::vector<VertexType>& vertices,
               const std::vector<ElementType>& elements);

  void SetDataAttribute(int index, int size, int type, int stride, int offset);

  void SetIndicesCount(size_t count) noexcept;
  void SetIndicesOffset(size_t offset) noexcept;

 private:
  uint32_t draw_type_ = 0;
  GLenum indices_type_ = 0;
  size_t indices_count_ = 0;  // Default indices count to size of input
  size_t indices_offset_ = 0;
  Buffer vbo_, ebo_;
};

#include <iostream>

template <typename VertexType, typename ElementType>
void RenderDrawElements::SetData(const std::vector<VertexType>& vertices,
                                 const std::vector<ElementType>& elements) {
  static_assert(std::unsigned_integral<ElementType>,
                "Elements must be unsigned!");

  vbo_.InsertData(vertices, GL_STATIC_DRAW);
  ebo_.InsertData(elements, GL_STATIC_DRAW);

  // Binds the EBO to VAO
  vao_.Bind();
  ebo_.Bind();
  vao_.Unbind();
  ebo_.Unbind();

  indices_type_ = GetDataType<ElementType>();
}
