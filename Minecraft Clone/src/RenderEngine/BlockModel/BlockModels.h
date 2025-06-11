// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

#include "Core/Typenames.h"

#define EAST 0x00
#define WEST 0x01
#define UP 0x02
#define DOWN 0x03
#define SOUTH 0x04
#define NORTH 0x05

enum DisplayPosition {
  thirdperson_righthand = 0,
  thirdperson_lefthand = 1,
  firstperson_righthand = 2,
  firstperson_lefthand = 3,
  gui = 4,
  head = 5,
  ground = 6,
  fixed = 7
};

struct BlockFace {
  BlockFace();
  ~BlockFace();
  BlockFace(const BlockFace&);
  BlockFace(BlockFace&&);
  BlockFace& operator=(const BlockFace&);

  glm::ivec3 normal_ = glm::ivec3(0, 0, 0);
  std::string reference_texture_ = "";  // variables starts with #
  int cull_face_ = -1;
  int tint_index_ = -1;
  size_t texture_id_ = 0;
  size_t texture_count_ = 0;  // For amination purposes.
  uint32_t rotation_ = 0;
  bool partially_transparent_pixel_ = false;
  bool fully_transparent_pixel_ = false;
  bool can_cull = false;
  glm::ivec4 uv_ = glm::ivec4(0, 0, 16, 16);

  glm::ivec2 uv_coord_00{0, 0};
  glm::ivec2 uv_coord_01{0, 0};
  glm::ivec2 uv_coord_11{0, 0};
  glm::ivec2 uv_coord_10{0, 0};
};

struct BlockDisplay {  // Positions {thirdperson_righthand,
                       // thirdperson_lefthand, firstperson_righthand,
                       // firstperson_lefthand, gui, head, ground, or fixed}
  BlockDisplay();
  ~BlockDisplay();
  BlockDisplay(const BlockDisplay&);
  BlockDisplay(BlockDisplay&&);
  BlockDisplay& operator=(const BlockDisplay&);

  // fixed = item frame
  DisplayPosition position_;
  glm::vec3 rotation_ = glm::vec3(0.f, 0.f, 0.f);
  glm::vec3 translation_ = glm::vec3(0.f, 0.f, 0.f);
  glm::vec3 scale_ = glm::vec3(0.f, 0.f, 0.f);
  bool initialized_ = false;
};

struct CuboidRotationInfo {
  CuboidRotationInfo();
  ~CuboidRotationInfo();
  CuboidRotationInfo(const CuboidRotationInfo&);
  CuboidRotationInfo(CuboidRotationInfo&&);
  CuboidRotationInfo& operator=(const CuboidRotationInfo&);

  glm::vec3 origin_ = glm::vec3(0.f, 0.f, 0.f);
  int axis_ = 0;
  int angle_ = 0;
  bool rescale_ = false;
  bool initialized_ = false;
};

struct Cuboid {
  Cuboid();
  ~Cuboid();
  Cuboid(const Cuboid&);
  Cuboid(Cuboid&&);
  Cuboid& operator=(const Cuboid&);

  void EditFace(int location, BlockFace f);
  // Relative inner block position from -16 to 32
  glm::vec3 from_ = glm::vec3(0, 0, 0);
  glm::vec3 to_ = glm::vec3(1, 1, 1);
  BlockFace faces_[6]{};
  CuboidRotationInfo rotation_;
  std::string comments_ = "";
  bool shade_ = true;
};

struct BlockModel {
  BlockModel();
  ~BlockModel();
  BlockModel(const BlockModel&);
  BlockModel(BlockModel&&);
  BlockModel& operator=(const BlockModel&);

  void AddDisplay(BlockDisplay display, DisplayPosition position);
  bool CheckDisplay(DisplayPosition position);
  void AddElement(Cuboid element);
  void GetVertices(std::vector<float>& vertices,
                   std::vector<uint32_t>& indices) const;
  void FlattenVariables();
  void BakeTextureRotation();

  std::vector<Cuboid> elements_{};
  bool ambient_occlusion_ = true;  // default value
  bool is_initialized_ = false;
  FastHashMap<std::string, std::string> texture_variable_;
  BlockDisplay display_[8]{};
};
