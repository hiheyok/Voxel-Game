#include "RenderEngine/UI/UIRenderer.h"

#include <cstddef>
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Render/RenderObject.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/UI/Data/UserInterfaceData.h"

using std::make_unique;
using std::vector;

UIRenderer::UIRenderer(GameContext& context)
    : context_{context},
      renderer_{context},
      handle_ssbo_{make_unique<Buffer>(context_)} {
  renderer_.SetDataAttribute(0, 2, GL_FLOAT, 9, 0);
  renderer_.SetDataAttribute(1, 2, GL_FLOAT, 9, 2);
  renderer_.SetDataAttribute(2, 4, GL_FLOAT, 9, 4);
  renderer_.SetDataAttribute(3, 1, GL_FLOAT, 9, 8);
  renderer_.SetShader("ui_render");

  handle_ssbo_->SetType(GL_SHADER_STORAGE_BUFFER);
  handle_ssbo_->SetUsage(GL_STATIC_DRAW);
}
UIRenderer::~UIRenderer() = default;

void UIRenderer::Submit(const vector<UIVertexFormat>& vertices,
                        const vector<uint32_t>& indices) {
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
  // Setup GL state for UI rendering
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  UploadToGPU();
  renderer_.GetShader()->BindBufferAsSSBO(handle_ssbo_->GetId(), 0);
  renderer_.Render();
  renderer_.GetShader()->BindBufferAsSSBO(0, 0);

  // Restore GL state
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
}
void UIRenderer::SetVirtualRes(glm::vec2 v_res) { v_res_ = v_res; }

void UIRenderer::SetScreenRes(glm::vec2 s_res) { s_res_ = s_res; }

void UIRenderer::UploadToGPU() {
  renderer_.SetData(vertices_, indices_);
  renderer_.SetIndicesCount(indices_.size());
  renderer_.GetShader()->SetVec2("v_res", v_res_);
  renderer_.GetShader()->SetVec2("s_res", s_res_);

  if (handles_dirty_) {
    handle_ssbo_->InsertData(handles_, GL_STATIC_DRAW);
    handles_dirty_ = false;
  }
}

GLuint64 UIRenderer::FindOrPutHandleIdx(GLuint64 handle) {
  for (size_t i = 0; i < handles_.size(); ++i) {
    if (handles_[i] == handle) {
      return i;
    }
  }

  handles_dirty_ = true;
  handles_.push_back(handle);
  return handles_.size() - 1;
}
