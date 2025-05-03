// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "RenderEngine/OpenGL/Render/RenderObject.h"

class RenderDrawArrays : public RenderObject {
 public:
  // Triangles are the default draw type
  explicit RenderDrawArrays(uint32_t draw_type = GL_TRIANGLES);
  ~RenderDrawArrays();

  void Render() override;

 private:
  uint32_t draw_type_ = 0;
};
