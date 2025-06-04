// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

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
class RenderDrawArrays;
class GameContext;

struct GLFWwindow;

class TextRenderer {
 public:
  explicit TextRenderer(GameContext&);
  ~TextRenderer();

  void InitializeTextRenderer(GLFWwindow* w);

  void InsertFontObject(const std::string& name, RenderableFont font);

  void RemoveFontObject(const std::string& name);

  void EditFontText(const std::string& name, const std::string& text);

  void ConstructBuffer();

  void Prepare();

  void RenderFont();

 private:
  GameContext& game_context_;
  FastHashMap<std::string, RenderableFont> font_map_;
  std::unique_ptr<Texture2D> font_texture_;
  std::unique_ptr<RenderDrawArrays> background_render_;
  std::unique_ptr<RenderDrawArrays> font_render_;
  GLFWwindow* window_ = nullptr;

  std::vector<float> vertices_ = {};
  std::vector<float> vertices_background_ = {};

  size_t vertices_count_ = 0;
  size_t background_vert_count_ = 0;
};
