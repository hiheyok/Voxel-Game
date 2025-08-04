#include "RenderEngine/UI/UIRenderer.h"

#include <cstdint>
#include <vector>

#include "RenderEngine/UI/Data/UserInterfaceData.h"

UIRenderer::UIRenderer(GameContext& game_context)
    : game_context_{game_context}, renderer_{game_context} {
  renderer_.SetDataAttribute(0, 2, GL_FLOAT, 6, 0);
  renderer_.SetDataAttribute(1, 4, GL_FLOAT, 6, 2);
  renderer_.SetShader("assets/shaders/UI/vert.glsl",
                      "assets/shaders/UI/frag.glsl");
}
UIRenderer::~UIRenderer() = default;

void UIRenderer::Submit(const std::vector<UIVertexFormat>& vertices,
                        const std::vector<uint32_t>& indices) {
  size_t index_offset = vertices_.size();
  size_t start = indices_.size();
  size_t end = start + indices.size();
  vertices_.insert(vertices_.end(), vertices.begin(), vertices.end());
  indices_.insert(indices_.end(), indices.begin(), indices.end());

  // Update the indices
  for (int i = start; i < end; ++i) {
    indices_[i] += index_offset;
  }

  is_dirty_ = true;
}

void UIRenderer::Clear() {
  vertices_.clear();
  indices_.clear();

  is_dirty_ = true;
}

void UIRenderer::Render() {
  UploadToGPU();
  renderer_.Render();
}

void UIRenderer::UploadToGPU() {
  renderer_.SetData(vertices_, indices_);
  renderer_.SetIndicesCount(indices_.size());
}
