#pragma once

#include <gl/glew.h>

#include <glm/vec2.hpp>

struct alignas(32) TextureSprite {
  glm::vec2 uv_beg_;
  glm::vec2 uv_end_;
  GLuint texture_id_;
  GLuint64 handle_;
};
