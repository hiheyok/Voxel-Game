#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
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
  void SetVirtualRes(glm::vec2 v_res);
  void SetScreenRes(glm::vec2 s_res);

 private:
  void UploadToGPU();
  bool is_dirty_ = false;

  GameContext& context_;

  std::vector<UIVertexFormat> vertices_;
  std::vector<uint32_t> indices_;

  RenderDrawElements renderer_;
  glm::vec2 v_res_ = {1920.0f, 1080.0f};
  glm::vec2 s_res_ = {1920.0f, 1080.0f};
};
