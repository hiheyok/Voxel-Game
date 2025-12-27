// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <array>

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

  void SetLightLvl(BlockPos pos, bool is_sky, int lvl);
  int GetLightLvl(BlockPos pos, bool is_sky) const;

  void SetBlock(BlockID block, BlockPos pos);
  void SetBlockUnsafe(BlockID block, BlockPos pos);

  void SetData(const ChunkRawData& data);
  ChunkRawData GetRawData();

  void SetPosition(ChunkPos pos) noexcept;

  void UpdateHeightMap();
  void UpdateHeightMap(int x, int z);

  // Check light update request

  // For light updates
  bool CheckLightDirty();
  void SetLightDirty();

  // Signal to relight entire chunk
  bool IsLightUp() const noexcept;
  void SetLightUp(bool);

  const Palette& GetPalette() const;

  GameContext& context_;
  ChunkPos position_;
  std::unique_ptr<LightStorage> sky_light_;
  std::unique_ptr<LightStorage> block_light_;
  std::unique_ptr<HeightMap> heightmap_;
  std::array<std::vector<SetBlockRelative>, 6> outside_block_to_place_;

  bool is_empty_ = true;

 protected:
  std::vector<std::optional<ChunkContainer*>> neighbors_;

 private:
  std::atomic<bool> light_dirty_ = false;
  std::atomic<bool> is_light_up_ = false;
  bool light_update_seeding =
      true;  // Parameters to seed newly generated chunks
  Palette block_storage_;
};
