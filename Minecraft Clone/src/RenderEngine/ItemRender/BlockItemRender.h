// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

#include "Assets/AssetHandle.h"

class VertexArray;
class Buffer;
class Shader;
class Camera;
class Item;
class GameContext;
class RenderDrawElements;
class RenderableModel;

class BlockItemRender {
  struct VertexFormat {
    glm::vec3 pos_;
    glm::vec2 uv_;
    float light_;
  };

 public:
  explicit BlockItemRender(GameContext&);
  ~BlockItemRender();

  void Initialize();
  void RenderBlock(Item item);

 private:
  GameContext& context_;
  std::unique_ptr<RenderDrawElements> render_;
  std::unique_ptr<Camera> camera_;

  void ExtractVertices(AssetHandle<RenderableModel> model,
                       std::vector<VertexFormat>& vertices,
                       std::vector<uint32_t>& indices);
  void SetCamera();
  void setDrawCalls();
};
