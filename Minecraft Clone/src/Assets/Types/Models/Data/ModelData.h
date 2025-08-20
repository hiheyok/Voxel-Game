#pragma once

#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <utility>

#include "Core/Typenames.h"

// namespace to groups all of the model data specific data structures
namespace model {
enum class DisplayPosition {
  kThirdPersonRighthand,
  kThirdPersonLefthand,
  kFirstPersonRighthand,
  kFirstPersonLefthand,
  kGui,
  kHead,
  kGround,
  kFixed,
  kOnShelf  // Item specific
};

// For display position
struct Transformation {
  glm::vec3 rotation_{0.0f, 0.0f, 0.0f};
  glm::vec3 translation_{0.0f, 0.0f, 0.0f};
  glm::vec3 scale_{1.0f, 1.0f, 1.0f};
};

// For the actual element
struct Rotation {
  glm::vec3 origin_{0.0f, 0.0f, 0.0f};
  float angle_ = 0.0f;
  bool rescale_ = false;  // Default: False
  char axis_;
};

struct Face {
  glm::vec4 uv_{0.0f, 0.0f, 0.0f, 0.0f};  // Optional
  std::string texture_;
  int cull_face_;
  int texture_rotation_ = 0;  // Default: 0
  int tint_index_ = -1;       // Default: -1
};

struct Element {
  std::string name_;  // Optional
  glm::vec3 from_{0.0f, 0.0f, 0.0f};
  glm::vec3 to_{16.0f, 16.0f, 16.0f};
  Rotation rotation_;
  bool shade_ = true;       // Default: true
  int light_emission_ = 0;  // Default: 0

  // 6 faces as there are only 6 possible faces
  std::array<Face, 6> faces_;
};

// Used for item model rendering
enum class GuiLight { kSide, kFront };

struct ModelData {
  bool ambient_occlusion_ = true;  // Default: True
  std::vector<Element> elements_;
  std::vector<std::pair<DisplayPosition, Transformation>> displays_;
  std::vector<std::pair<std::string, std::string>> texture_variables_;
  // Used for item models
  GuiLight gui_light_ = GuiLight::kSide;  // Default: kSide
};

}  // namespace model
