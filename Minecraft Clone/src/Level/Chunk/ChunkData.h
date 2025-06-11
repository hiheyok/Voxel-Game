// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <atomic>
#include <glm/vec3.hpp>
#include <optional>
#include <vector>

#include "Core/Typenames.h"
#include "Level/Chunk/Palette.h"
#include "Level/Light/LightStorage.h"

class HeightMap;
class LightStorage;
class GameContext;

struct ChunkRawData;
struct SetBlockRelative;

class ChunkContainer {
 public:
  explicit ChunkContainer(GameContext&);
  virtual ~ChunkContainer();
  ChunkContainer(GameContext&, const ChunkRawData&);
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

  const Palette& GetPalette() const;

  GameContext& game_context_;
  ChunkPos position_;
  std::unique_ptr<LightStorage> lighting_;
  std::unique_ptr<HeightMap> heightmap_;
  std::vector<std::vector<SetBlockRelative>> outside_block_to_place_;

  bool is_empty_ = true;

 protected:
  std::vector<std::optional<ChunkContainer*>> neighbors_;

 private:
  std::atomic<bool> light_dirty_ = false;
  Palette block_storage_;
};
