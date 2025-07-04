// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <gl/glew.h>

#include <memory>
#include <string>
#include <vector>
#include <glm/vec2.hpp>

#include "Core/Typenames.h"

class Buffer;
class VertexArray;
class Shader;
class Texture2D;
class GameContext;

struct GUIElement;

class GUISet {
 public:
  explicit GUISet(GameContext&);
  ~GUISet();

  GUISet(GUISet&&);
  GUISet(const GUISet&) = delete;

  void Clean();
  void AddGUIElementNorm(std::string Name, std::string Text, glm::vec2 Size,
                         glm::vec2 Position, glm::vec2 UV_P0, glm::vec2 UV_P1);
  void AddGUIElement(std::string name, std::string text, glm::vec2 size,
                     glm::vec2 position, glm::vec2 UV_P0, glm::vec2 UV_P1);
  void EditElementPosition(std::string Name, glm::vec2 Position);
  void EditElementUVNorm(std::string Name, glm::vec2 UV0, glm::vec2 UV1);
  void SetGUITexture(std::string file);
  void SetGUITexture(GLuint texture_id_, size_t x, size_t y);
  void PrepareRenderer();
  size_t GetNumRenderableObjects() const;
  GLuint GetGUITextureID() const;
  
  GameContext& game_context_;
  std::vector<Buffer> ebos_;
  std::vector<VertexArray> vaos_;
  std::vector<size_t> vbo_size_;

 private:
  size_t AddRenderingObj();

  int num_of_renderable_objects_ = 0;
  FastHashMap<std::string, int> gui_element_index_;
  bool is_dirty_ = true;
  std::unique_ptr<Texture2D> gui_texture_;
  std::vector<Buffer> vbos_;
  std::vector<GUIElement> elements_;
};
