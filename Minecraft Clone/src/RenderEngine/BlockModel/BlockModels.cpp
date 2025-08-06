// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/BlockModel/BlockModels.h"

#include <string>
#include <utility>
#include <vector>

#include "Core/Typenames.h"
#include "Utils/LogUtils.h"

// BlockFace struct

BlockFace::BlockFace() = default;
BlockFace::~BlockFace() = default;
BlockFace::BlockFace(const BlockFace&) = default;
BlockFace::BlockFace(BlockFace&&) = default;
BlockFace& BlockFace::operator=(const BlockFace&) = default;

// BlockDisplay struct

BlockDisplay::BlockDisplay() = default;
BlockDisplay::~BlockDisplay() = default;
BlockDisplay::BlockDisplay(const BlockDisplay&) = default;
BlockDisplay::BlockDisplay(BlockDisplay&&) = default;
BlockDisplay& BlockDisplay::operator=(const BlockDisplay&) = default;

// CuboidRotationInfo struct

CuboidRotationInfo::CuboidRotationInfo() = default;
CuboidRotationInfo::~CuboidRotationInfo() = default;
CuboidRotationInfo::CuboidRotationInfo(const CuboidRotationInfo&) = default;
CuboidRotationInfo::CuboidRotationInfo(CuboidRotationInfo&&) = default;
CuboidRotationInfo& CuboidRotationInfo::operator=(const CuboidRotationInfo&) =
    default;

// Cuboid struct

Cuboid::Cuboid() = default;
Cuboid::~Cuboid() = default;
Cuboid::Cuboid(const Cuboid&) = default;
Cuboid::Cuboid(Cuboid&&) = default;
Cuboid& Cuboid::operator=(const Cuboid&) = default;

void Cuboid::EditFace(int location, BlockFace f) { faces_[location] = f; }

// BlockModel struct

BlockModel::BlockModel() = default;
BlockModel::~BlockModel() = default;
BlockModel::BlockModel(const BlockModel&) = default;
BlockModel::BlockModel(BlockModel&&) = default;
BlockModel& BlockModel::operator=(const BlockModel&) = default;

void BlockModel::AddDisplay(BlockDisplay display, DisplayPosition position) {
  display_[position] = display;
}

bool BlockModel::CheckDisplay(DisplayPosition position) {
  return display_[position].initialized_;
}

void BlockModel::AddElement(Cuboid element) { elements_.push_back(element); }

void BlockModel::GetVertices(std::vector<float>& vertices,
                             std::vector<uint32_t>& indices) const {
  for (const Cuboid& element : elements_) {
    glm::vec3 from = element.from_;
    glm::vec3 to = element.to_;
    for (auto side : Directions<BlockPos>()) {
      BlockFace face = element.faces_[side];
      if (face.reference_texture_.length() == 0) continue;

      glm::vec2 uv0{face.uv_.x, face.uv_.y};
      glm::vec2 uv1{face.uv_.z, face.uv_.w};

      uv0 = uv0 / 16.f;
      uv1 = uv1 / 16.f;

      uint8_t axis = side.GetAxis();

      uint32_t currIndex = static_cast<uint32_t>(vertices.size()) / 7;

      switch (axis) {
        case 0:
          side.IsNegative()
              ? vertices.insert(vertices.end(),
                                {
                                    from.x,
                                    from.y,
                                    from.z,
                                    uv1.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    8.f,
                                    from.x,
                                    to.y,
                                    from.z,
                                    uv1.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    8.f,
                                    from.x,
                                    to.y,
                                    to.z,
                                    uv0.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    8.f,
                                    from.x,
                                    from.y,
                                    to.z,
                                    uv0.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    8.f,
                                })
              : vertices.insert(vertices.end(),
                                {
                                    to.x,
                                    from.y,
                                    from.z,
                                    uv1.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    8.f,
                                    to.x,
                                    to.y,
                                    from.z,
                                    uv1.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    8.f,
                                    to.x,
                                    to.y,
                                    to.z,
                                    uv0.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    8.f,
                                    to.x,
                                    from.y,
                                    to.z,
                                    uv0.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    8.f,
                                });
          break;
        case 1:
          side.IsNegative()
              ? vertices.insert(vertices.end(),
                                {
                                    from.x,
                                    from.y,
                                    from.z,
                                    uv1.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    15.f,
                                    to.x,
                                    from.y,
                                    from.z,
                                    uv0.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    15.f,
                                    to.x,
                                    from.y,
                                    to.z,
                                    uv0.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    15.f,
                                    from.x,
                                    from.y,
                                    to.z,
                                    uv1.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    15.f,
                                })
              : vertices.insert(vertices.end(),
                                {
                                    from.x,
                                    to.y,
                                    from.z,
                                    uv1.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    15.f,
                                    to.x,
                                    to.y,
                                    from.z,
                                    uv0.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    15.f,
                                    to.x,
                                    to.y,
                                    to.z,
                                    uv0.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    15.f,
                                    from.x,
                                    to.y,
                                    to.z,
                                    uv1.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    15.f,
                                });
          break;
        case 2:
          side.IsNegative()
              ? vertices.insert(vertices.end(),
                                {
                                    from.x,
                                    from.y,
                                    from.z,
                                    uv1.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    12.f,
                                    to.x,
                                    from.y,
                                    from.z,
                                    uv0.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    12.f,
                                    to.x,
                                    to.y,
                                    from.z,
                                    uv0.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    12.f,
                                    from.x,
                                    to.y,
                                    from.z,
                                    uv1.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    12.f,
                                })
              : vertices.insert(vertices.end(),
                                {
                                    from.x,
                                    from.y,
                                    to.z,
                                    uv1.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    12.f,
                                    to.x,
                                    from.y,
                                    to.z,
                                    uv0.x,
                                    uv0.y,
                                    static_cast<float>(face.texture_id_),
                                    12.f,
                                    to.x,
                                    to.y,
                                    to.z,
                                    uv0.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    12.f,
                                    from.x,
                                    to.y,
                                    to.z,
                                    uv1.x,
                                    uv1.y,
                                    static_cast<float>(face.texture_id_),
                                    12.f,
                                });
          break;
      }

      if (side.IsPositive()) {
        indices.insert(indices.end(),
                       {0 + currIndex, 1 + currIndex, 2 + currIndex,
                        2 + currIndex, 3 + currIndex, 0 + currIndex});
      }
    }
  }
}

void BlockModel::BakeTextureRotation() {
  for (Cuboid& element : elements_) {
    for (auto direction : Directions<BlockPos>()) {
      BlockFace& face = element.faces_[direction];
      if (face.reference_texture_.length() == 0)
        continue;  // means not initialized

      int rotation_ = face.rotation_;

      face.uv_coord_00 = {face.uv_.x, face.uv_.y};
      face.uv_coord_01 = {face.uv_.x, face.uv_.w};
      face.uv_coord_11 = {face.uv_.z, face.uv_.w};
      face.uv_coord_10 = {face.uv_.z, face.uv_.y};

      for (int r = 0; r < rotation_; r += 90) {
        std::swap(face.uv_coord_00, face.uv_coord_01);
        std::swap(face.uv_coord_00, face.uv_coord_10);
        std::swap(face.uv_coord_10, face.uv_coord_11);
      }

      if (direction.GetAxis() == kZAxis) {
        std::swap(face.uv_coord_10, face.uv_coord_01);
      }
    }
  }
}

void BlockModel::FlattenVariables() {
  FastHashMap<std::string, std::string> variableMatcher{};
  FastHashSet<std::string> variableNames{};

  // This will set all of the texture variables

  for (const auto& [key, value] : texture_variable_) {
    // Check if there is a # at the beginning
    variableNames.insert('#' + key);
    if (value[0] == '#') {
      variableNames.insert(value);
    }

    // In this case, i am inserting both key and values because a variable
    // could be referencing another variable
  }

  // Then it will find the absolute value corresponding to those variables

  for (const std::string& variable : variableNames) {
    std::string textureName = variable;
    std::vector<std::string> variableCorrspondingToSameTexture = {};

    FastHashSet<std::string> variableVisitMap{};

    int i = 0;
    while (textureName[0] == '#') {
      if (variableMatcher.count(textureName)) {  // top down memorization
        textureName = variableMatcher[textureName];
        break;
      }
      if (i > 10000) {  // TODO(hiheyok): Investigate this infinite loop bug 
        break;
      }
      i++;
      variableCorrspondingToSameTexture.push_back(textureName);
      textureName =
          texture_variable_[textureName.substr(1, textureName.length() - 1)];
    }

    for (const std::string& var : variableCorrspondingToSameTexture) {
      variableMatcher[var] = textureName;
    }
    variableCorrspondingToSameTexture.clear();
  }

  // After all of the texture are flatten it will go through the faces and set
  // the textures
  for (Cuboid& element : elements_) {
    for (BlockFace& face : element.faces_) {
      if (face.reference_texture_.empty())
        continue;  // if there is no texture face is not initialized
                   // (skip)
      if (face.reference_texture_[0] != '#') continue;
      face.reference_texture_ = variableMatcher[face.reference_texture_];
    }
  }
}
