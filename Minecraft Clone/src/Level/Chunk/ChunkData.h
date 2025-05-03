// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec3.hpp>
#include <optional>
#include <vector>

#include "Core/Typenames.h"
#include "Level/Chunk/Palette.h"
#include "Level/Light/LightStorage.h"

class HeightMap;
class LightStorage;

struct ChunkRawData;
struct SetBlockRelative;

class ChunkContainer {
 public:
  ChunkContainer();
  virtual ~ChunkContainer();
  ChunkContainer(const ChunkRawData&);
  ChunkContainer(ChunkContainer&&);
  ChunkContainer(const ChunkContainer&) = delete;

  void SetNeighbor(ChunkContainer* neighbor, int side);
  std::optional<ChunkContainer*> GetNeighbor(int side) const noexcept;
  void ClearNeighbors();

  BlockID GetBlock(BlockPos pos) const;
  BlockID GetBlockUnsafe(BlockPos pos) const noexcept;

  void SetBlock(BlockID block, BlockPos pos);
  void SetBlockUnsafe(BlockID block, BlockPos pos);

  void SetData(const ChunkRawData& data);
  ChunkRawData GetRawData();
  LightStorage GetLightData();

  void SetPosition(ChunkPos pos) noexcept;

  void UpdateHeightMap();
  void UpdateHeightMap(int x, int z);

  // Check light update request

  bool CheckLightDirty();
  void SetLightDirty();

  ChunkPos position_;
  std::unique_ptr<LightStorage> lighting_;
  std::unique_ptr<HeightMap> heightmap_;
  bool is_empty_ = true;
  std::vector<std::vector<SetBlockRelative>> outside_block_to_place_;

 protected:
  std::vector<std::optional<ChunkContainer*>> neighbors_;

 private:
  bool light_dirty_ = false;
  Palette block_storage_;
};
