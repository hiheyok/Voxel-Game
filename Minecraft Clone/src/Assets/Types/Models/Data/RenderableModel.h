#pragma once
#include <array>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <optional>
#include <vector>

#include "Assets/Asset.h"

// This is used for storing the final data to be pipelined into the meshing or
// item rendering engine

class GameContext;

namespace baked_model {

struct Face {
  // Sprite data
  glm::vec4 uv_;
  int cull_face_;
  int tint_index_ = -1;
};

struct Element {
  // Only used for fast culling checks on non-rotated models
  glm::vec3 from_, to_;

  // Store the final pre-rotated model in a form of coordinates relative to
  // block-local space [0,1]
  // corners indexed convention
  // 0b000 -> 0b111
  // 0b[x][y][z] where 1 means to_ and 0 means from_
  std::array<glm::vec3, 8> corners_;
  std::array<std::optional<Face>, 6> faces_;
  int min_light_emission_ = 0;
  bool shade_ = true;
};

}  // namespace baked_model

class RenderableModel : public Asset {
 public:
  RenderableModel(GameContext&, const std::string& key);
  ~RenderableModel();

  const std::vector<baked_model::Element>& GetElements() const noexcept;
  bool CheckAmbientOcclusion() const noexcept;

  // TODO(hiheyok): Add some display position code
 private:
  friend class ModelManager;

  void Load() override {};

  GameContext& context_;
  std::vector<baked_model::Element> elements;
  bool ambient_occlusion_ = true;
};
