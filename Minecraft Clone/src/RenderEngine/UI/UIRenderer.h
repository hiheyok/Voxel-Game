#pragma once

#include <cstdint>
#include <vector>

#include "RenderEngine/OpenGL/Render/RenderDrawElements.h"

class GameContext;
struct UIVertexFormat;

class UIRenderer {
 public:
  explicit UIRenderer(GameContext&);
  ~UIRenderer();
  // TODO(hiheyok): later add texture id
  void Submit(const std::vector<UIVertexFormat>& vertices,
              const std::vector<uint32_t>& indices);
  void Clear();
  void Render();

 private:
  void UploadToGPU();
  bool is_dirty_ = false;

  GameContext& game_context_;

  std::vector<UIVertexFormat> vertices_;
  std::vector<uint32_t> indices_;

  RenderDrawElements renderer_;
};
