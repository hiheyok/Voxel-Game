#pragma once

#ifndef CHUNKDATA_H
#define CHUNKDATA_H

#include <glm/vec3.hpp>
#include <vector>

#include "Core/Typenames.h"
#include "Level/Chunk/Palette.h"
#include "Level/Light/LightStorage.h"

class Heightmap;
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

  void SetNeighbor(ChunkContainer* neighbor, uint32_t side);
  ChunkContainer* GetNeighbor(uint32_t side) const;
  void ClearNeighbors();

  BlockID GetBlock(BlockPos pos) const;
  BlockID GetBlockUnsafe(BlockPos pos) const;

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
  std::vector<ChunkContainer*> neighbors_;
  std::vector<std::vector<SetBlockRelative>> outside_block_to_place_;
  std::unique_ptr<LightStorage> lighting_;
  std::unique_ptr<Heightmap> heightmap_;
  bool is_empty_ = true;

 private:
  bool light_dirty_ = false;
  Palette block_storage_;
};

#endif
