// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <array>
#include <bitset>
#include <glm/ext.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Core/Typenames.h"

class Chunk;
class GameContext;

struct Cuboid;
struct BlockFace;
struct BlockModel;
struct BlockVertexFormat;

namespace Mesh {
struct ChunkVertexData {
  ChunkVertexData() = default;
  ~ChunkVertexData() = default;

  std::vector<BlockVertexFormat> solid_vertices_;
  std::vector<BlockVertexFormat> transparent_vertices_;
  std::vector<BlockVertexFormat> fluid_vertices_buffer_;

  ChunkPos position_;
};

class ChunkMeshData {
 public:
  explicit ChunkMeshData(GameContext&);
  ~ChunkMeshData();

  void SetChunk(Chunk* chunk);
  void Reset();

  // Generate the Mesh
  void GenerateMesh();

  GameContext& game_context_;
  // Mesh Vertices
  std::vector<BlockVertexFormat> vertices_buffer_;
  std::vector<BlockVertexFormat> transparent_vertices_buffer_;
  std::vector<BlockVertexFormat> solid_fluid_vertices_buffer_;
  std::vector<BlockVertexFormat> transparent_fluid_vertices_buffer_;

  size_t transparent_face_count_ = 0;
  size_t solid_face_count_ = 0;
  size_t solid_fluid_face_count_ = 0;
  size_t transparent_fluid_face_count_ = 0;

  size_t greedy_time_;
  size_t cache_time_;

 private:
  // Used to generate the cache
  void GenerateCache();

  // Generates all the faces and puts them in the cache
  void GenerateFaceCollection();

  // Fluid mesh
  void GenerateFluidMesh();

  // Check if the player can see the mesh
  bool IsFaceVisible(const Cuboid& cube, BlockPos pos, uint8_t side);

  bool CompareBlockSide(BlockPos pos, uint8_t side, BlockID b);

  // Add faces to the mesh
  void AddFaceToMesh(const BlockFace& face, uint8_t axis, glm::vec3 from,
                     glm::vec3 to, bool allow_ao, BlockPos pos);

  const BlockID& GetCachedBlockID(BlockPos pos) const noexcept;
  void SetCachedBlockID(BlockID b, BlockPos pos) noexcept;

  void GetAO(int direction, BlockPos pos, int& ao_00, int& ao_01,
             int& ao_10, int& ao_11);

  static constexpr uint64_t kBufferStepSize = 4096;

  Chunk* chunk_;

  // TODO(hiheyok): 3 x is to use cache locality for processing for each axis
  // (haven't implemented yet)
  std::array<BlockID, (kChunkDim + 2) * (kChunkDim + 2) * (kChunkDim + 2)>
      chunk_cache_;
  std::bitset<(kChunkDim + 2) * (kChunkDim + 2) * (kChunkDim + 2)> is_fluid_;
};
}  // namespace Mesh
