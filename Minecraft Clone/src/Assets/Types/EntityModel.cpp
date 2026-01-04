// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Assets/Types/EntityModel.h"

#include <array>
#include <cstddef>
#include <string>

#include "Assets/Asset.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"

using std::array;
using std::string;

static constexpr std::array<int, 36> kIndices = {
    // Front
    0,
    1,
    2,
    2,
    3,
    0,

    // Back
    4,
    6,
    5,
    6,
    4,
    7,

    // Right
    8,
    9,
    10,
    10,
    11,
    8,

    // Left
    12,
    14,
    13,
    14,
    12,
    15,

    // Top
    16,
    17,
    18,
    18,
    19,
    16,

    // Bottom
    20,
    22,
    21,
    22,
    20,
    23,
};

Model::ModelData Model::RectangularPrism::GetVertices() {
  ModelData model;

  model.vertices_.insert(
      model.vertices_.begin(),
      {                // Front
       0.f, 0.f, 0.f,  // 0
       uv_map_[Directions<BlockPos>::kEast].p1_.x,
       uv_map_[Directions<BlockPos>::kEast].p0_.y, size_.x, 0.f, 0.f,  // 1
       uv_map_[Directions<BlockPos>::kEast].p0_.x,
       uv_map_[Directions<BlockPos>::kEast].p0_.y, size_.x, size_.y,
       0.f,  // 2
       uv_map_[Directions<BlockPos>::kEast].p0_.x,
       uv_map_[Directions<BlockPos>::kEast].p1_.y, 0.f, size_.y, 0.f,  // 3
       uv_map_[Directions<BlockPos>::kEast].p1_.x,
       uv_map_[Directions<BlockPos>::kEast].p1_.y,

       // Back
       0.f, 0.f, size_.z,  // 4
       uv_map_[Directions<BlockPos>::kWest].p1_.x,
       uv_map_[Directions<BlockPos>::kWest].p0_.y, size_.x, 0.f,
       size_.z,  // 5
       uv_map_[Directions<BlockPos>::kWest].p0_.x,
       uv_map_[Directions<BlockPos>::kWest].p0_.y, size_.x, size_.y,
       size_.z,  // 6
       uv_map_[Directions<BlockPos>::kWest].p0_.x,
       uv_map_[Directions<BlockPos>::kWest].p1_.y, 0.f, size_.y,
       size_.z,  // 7
       uv_map_[Directions<BlockPos>::kWest].p1_.x,
       uv_map_[Directions<BlockPos>::kWest].p1_.y,

       // Right
       size_.x, 0.f, 0.f,  // 8
       uv_map_[Directions<BlockPos>::kSouth].p1_.x,
       uv_map_[Directions<BlockPos>::kSouth].p0_.y, size_.x, size_.y,
       0.f,  // 9
       uv_map_[Directions<BlockPos>::kSouth].p1_.x,
       uv_map_[Directions<BlockPos>::kSouth].p1_.y, size_.x, size_.y,
       size_.z,  // 10
       uv_map_[Directions<BlockPos>::kSouth].p0_.x,
       uv_map_[Directions<BlockPos>::kSouth].p1_.y, size_.x, 0.f,
       size_.z,  // 11
       uv_map_[Directions<BlockPos>::kSouth].p0_.x,
       uv_map_[Directions<BlockPos>::kSouth].p0_.y,

       // Left
       0.f, 0.f, 0.f,  // 12
       uv_map_[Directions<BlockPos>::kNorth].p1_.x,
       uv_map_[Directions<BlockPos>::kNorth].p0_.y, 0.f, size_.y, 0.f,  // 13
       uv_map_[Directions<BlockPos>::kNorth].p1_.x,
       uv_map_[Directions<BlockPos>::kNorth].p1_.y, 0.f, size_.y,
       size_.z,  // 14
       uv_map_[Directions<BlockPos>::kNorth].p0_.x,
       uv_map_[Directions<BlockPos>::kNorth].p1_.y, 0.f, 0.f, size_.z,  // 15
       uv_map_[Directions<BlockPos>::kNorth].p0_.x,
       uv_map_[Directions<BlockPos>::kNorth].p0_.y,

       // Top
       0.f, size_.y, 0.f,  // 16
       uv_map_[Directions<BlockPos>::kUp].p1_.x,
       uv_map_[Directions<BlockPos>::kUp].p0_.y, size_.x, size_.y, 0.f,  // 17
       uv_map_[Directions<BlockPos>::kUp].p0_.x,
       uv_map_[Directions<BlockPos>::kUp].p0_.y, size_.x, size_.y,
       size_.z,  // 18
       uv_map_[Directions<BlockPos>::kUp].p0_.x,
       uv_map_[Directions<BlockPos>::kUp].p1_.y, 0.f, size_.y, size_.z,  // 19
       uv_map_[Directions<BlockPos>::kUp].p1_.x,
       uv_map_[Directions<BlockPos>::kUp].p1_.y,

       // Bottom
       0.f, 0.f, 0.f,  // 20
       uv_map_[Directions<BlockPos>::kDown].p1_.x,
       uv_map_[Directions<BlockPos>::kDown].p0_.y, size_.x, 0.f, 0.f,  // 21
       uv_map_[Directions<BlockPos>::kDown].p0_.x,
       uv_map_[Directions<BlockPos>::kDown].p0_.y, size_.x, 0.f,
       size_.z,  // 22
       uv_map_[Directions<BlockPos>::kDown].p0_.x,
       uv_map_[Directions<BlockPos>::kDown].p1_.y, 0.f, 0.f, size_.z,  // 23
       uv_map_[Directions<BlockPos>::kDown].p1_.x,
       uv_map_[Directions<BlockPos>::kDown].p1_.y});

  model.indices_.insert(model.indices_.begin(), kIndices.begin(),
                        kIndices.end());

  model.vertices_count_ = 24;

  return model;
}

Model::RectangularPrism::RectangularPrism(float x, float y, float z) {
  size_.x = x;
  size_.y = y;
  size_.z = z;
}

Model::RectangularPrism::RectangularPrism(glm::vec3 size, glm::vec3 offset) {
  size_ = size;
  offset_ = offset;
}

Model::RectangularPrism* EntityModel::AddRectangle(glm::vec3 size,
                                                   glm::vec3 offset) {
  shapes_.emplace_back(size, offset);
  return &shapes_.back();
}

EntityModel::EntityModel(const string& key) : Asset{key} {}

Model::ModelData EntityModel::GetVertices() {
  Model::ModelData model;

  for (Model::RectangularPrism& shape : shapes_) {
    Model::ModelData SubModel = shape.GetVertices();

    size_t CurrentIndex = model.vertices_count_;

    for (size_t i = 0; i < SubModel.vertices_.size(); i++) {
      if ((i % 5) < 3) {
        model.vertices_.push_back(SubModel.vertices_[i] + shape.offset_[i % 5]);
      } else {
        model.vertices_.push_back(SubModel.vertices_[i]);
      }
    }

    for (size_t i = 0; i < SubModel.indices_.size(); i++) {
      model.indices_.push_back(SubModel.indices_[i] + CurrentIndex);
    }

    model.vertices_count_ += SubModel.vertices_count_;
  }

  return model;
}

void EntityModel::Clear() { shapes_.clear(); }
