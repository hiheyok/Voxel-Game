// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>

class VertexArray;
class Buffer;
class Shader;
class Camera;
class Item;
class GameContext;
class RenderDrawElements;

class BlockItemRender {
 public:
  explicit BlockItemRender(GameContext&);
  ~BlockItemRender();

  void Initialize();
  void RenderBlock(Item item);

 private:

  GameContext& game_context_;
  std::unique_ptr<RenderDrawElements> render_;
  std::unique_ptr<Camera> camera_;

  void SetCamera();
  void setDrawCalls();
};
