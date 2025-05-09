// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Render/RenderDrawArrays.h"

RenderDrawArrays::RenderDrawArrays(uint32_t draw_type) {
  draw_type_ = draw_type;
}

RenderDrawArrays::~RenderDrawArrays() = default;

void RenderDrawArrays::Render() {}
