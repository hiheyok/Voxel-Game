// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>

class VertexArray;
class Buffer;
class Shader;
class Camera;
class Item;
class GameContext;

class BlockItemRender {
 public:
  explicit BlockItemRender(GameContext&);
  ~BlockItemRender();

  void Initialize();
  void RenderBlock(Item item);

 private:

  GameContext& game_context_;
  std::unique_ptr<VertexArray> vao_;
  std::unique_ptr<Buffer> ebo_, vbo_;
  std::unique_ptr<Shader> shader_;
  std::unique_ptr<Camera> camera_;

  void SetCamera();
  void setDrawCalls();
};
