// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"

#include <memory>

#include "Assets/AssetManager.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "Utils/LogUtils.h"

TexturedFrameBuffer::TexturedFrameBuffer(GameContext& context)
    : context_{context} {}
TexturedFrameBuffer::~TexturedFrameBuffer() = default;

void TexturedFrameBuffer::GenBuffer(GLint x, GLint y, float muti,
                                    GLuint format) {
  screen_ = std::make_unique<Shader>(
      context_, *context_.assets_->GetShaderSource("fb_render"));
  screen_->SetVec2("Resolution", {x, y});

  sy = y;
  sx = x;

  text_size_x_ = sx;
  text_size_y_ = sy;

  res_multiplier_ = muti;
  glGenFramebuffers(1, &fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  // create a color attachment texture
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, format, static_cast<int>(muti * x),
               static_cast<int>(muti * y), 0, format, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_, 0);
  // create a renderbuffer object for depth and stencil attachment (we won't
  // be sampling these)
  glGenRenderbuffers(1, &rbo_);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
  glRenderbufferStorage(
      GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<int>(muti * x),
      static_cast<int>(muti * y));  // use a single renderbuffer object for
                                    // both a depth AND stencil buffer.

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo_);  // now actually attach it
  // now that we actually created the framebuffer and added all attachments we
  // want to check if it is actually complete now
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error(
        "TexturedFrameBuffer::GenBuffer - Failed to create framebuffer!");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  screen_->BindTexture2D(0, texture_, "screenTexture");
  shader_id_ = screen_->shader_id_;
  float quadVertices[] = {
      // vertex attributes for a quad that fills the entire screen in
      // Normalized Device Coordinates. NOTE that this plane is now much
      // smaller and at the top of the screen
      // positions   // texCoords
      -1.f, 1.0f, 0.0f, 1.0f, -1.f, -1.f,  0.0f, 0.0f, 1.f, -1.f, 1.0f, 0.0f,

      -1.f, 1.0f, 0.0f, 1.0f, 1.f,  -1.0f, 1.0f, 0.0f, 1.f, 1.0f, 1.0f, 1.0f};

  glGenVertexArrays(1, &quad_vao_);
  glGenBuffers(1, &quad_vbo_);
  glBindVertexArray(quad_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void*>(2 * sizeof(float)));

  context_.logger_->LogDebug(
      "TexturedFrameBuffer::GenBuffer",
      "Created new framebuffer: " + std::to_string(fbo_));
}

void TexturedFrameBuffer::UpdateResolution(GLint x, GLint y, float muti) {
  sx = static_cast<GLint>(muti * x);
  sy = static_cast<GLint>(muti * y);
}

void TexturedFrameBuffer::BindRBO() const {
  glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
}

void TexturedFrameBuffer::UnbindRBO() const {
  glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
}

void TexturedFrameBuffer::BindFBO() {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glViewport(0, 0, static_cast<int>(res_multiplier_ * sx),
             static_cast<int>(res_multiplier_ * sy));
  if (sx != text_size_x_ || sy != text_size_y_) {
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, static_cast<int>(res_multiplier_ * sx),
        static_cast<int>(res_multiplier_ * sy), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          static_cast<int>(res_multiplier_ * sx),
                          static_cast<int>(res_multiplier_ * sy));
    text_size_y_ = sy;
    text_size_x_ = sx;
  }
}

void TexturedFrameBuffer::UnbindFBO() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, sx, sy);
  if (sx != text_size_x_ || sy != text_size_y_) {
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, static_cast<int>(res_multiplier_ * sx),
        static_cast<int>(res_multiplier_ * sy), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          static_cast<int>(res_multiplier_ * sx),
                          static_cast<int>(res_multiplier_ * sy));
    text_size_y_ = sy;
    text_size_x_ = sx;
  }
}

void TexturedFrameBuffer::Clear() const {
  glDeleteFramebuffers(1, &fbo_);
  glDeleteTextures(1, &texture_);
  glDeleteRenderbuffers(1, &rbo_);
}

void TexturedFrameBuffer::GetShaders() {}

void TexturedFrameBuffer::UpdateTexture() {}

void TexturedFrameBuffer::Render() {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glUseProgram(shader_id_);
  glBindVertexArray(quad_vao_);
  glBindTexture(GL_TEXTURE_2D, texture_);  // use the color attachment texture
                                           // as the texture of the quad plane
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}
