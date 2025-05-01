#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Core/Typenames.h"

class VertexArray;
class Buffer;
class Shader;
class RenderableFont;
class Texture2D;

struct GLFWwindow;

// TODO: Rename
class TextRenderer {
 public:
  TextRenderer();
  ~TextRenderer();

  void InitializeTextRenderer(GLFWwindow* w);

  void InsertFontObject(const std::string& name, RenderableFont font);

  void RemoveFontObject(const std::string& name);

  void EditFontText(const std::string& name, const std::string& text);

  void ConstructBuffer();

  void Prepare();

  void RenderFont();

 private:
  FastHashMap<std::string, RenderableFont> font_map_;
  std::unique_ptr<Shader> font_shader_, background_shader_;
  std::unique_ptr<Buffer> vbo_, background_vbo_;
  std::unique_ptr<VertexArray> vao_, background_vao_;
  std::unique_ptr<Texture2D> font_texture_;
  GLFWwindow* window_ = nullptr;

  std::vector<float> vertices_ = {};
  std::vector<float> vertices_background_ = {};

  size_t vertices_count_ = 0;
  size_t background_vert_count_ = 0;
};
