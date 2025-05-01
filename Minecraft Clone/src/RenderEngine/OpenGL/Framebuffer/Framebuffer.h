#pragma once
#include <gl/glew.h>

#include <memory>
#include <vector>

class Shader;

// TODO: Make this a child of texture class ??
class TexturedFrameBuffer {
 public:
  TexturedFrameBuffer();
  ~TexturedFrameBuffer();

  void GenBuffer(GLint x, GLint y, float muti, GLuint format = GL_RGBA);

  void UpdateResolution(GLint x, GLint y, float muti = 1.f);

  void BindRBO() const;

  void UnbindRBO() const;

  void BindFBO();

  void UnbindFBO();

  void Clear() const;

  void GetShaders();

  void UpdateTexture();

  void Render();

  GLuint texture_ = 0;

 private:
  std::unique_ptr<Shader> screen_;
  GLuint fbo_ = 0, rbo_ = 0, shader_id_ = 0;
  GLint sx = 0;
  GLint sy = 0;

  GLint text_size_x_ = 0, text_size_y_ = 0;
  uint32_t quad_vao_ = 0, quad_vbo_ = 0;
  GLfloat res_multiplier_ = 1;
};
