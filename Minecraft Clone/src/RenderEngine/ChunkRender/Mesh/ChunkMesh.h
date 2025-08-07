// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <array>
#include <bitset>
#include <glm/ext.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "Core/Typenames.h"

#if defined(_MSC_VER)
#include <immintrin.h>
#define restrict
#elif defined(__GNUC__) || defined(__clang__)
#include <x86intrin.h>
#define restrict __restrict__
#else
#define restrict
#endif

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

  GameContext& context_;
  // Mesh Vertices
  std::vector<BlockVertexFormat> solid_vertices_buffer_;
  std::vector<BlockVertexFormat> trans_vertices_buffer_;
  std::vector<BlockVertexFormat> solid_fluid_vertices_buffer_;
  std::vector<BlockVertexFormat> transparent_fluid_vertices_buffer_;

  size_t transparent_face_count_ = 0;
  size_t solid_face_count_ = 0;
  size_t solid_fluid_face_count_ = 0;
  size_t transparent_fluid_face_count_ = 0;

  size_t greedy_time_;
  size_t cache_time_;
  size_t add_face_clock_ = 0;
  size_t add_face_call_count_ = 0;

  bool use_option_ = false;  // for performance debugging purposes

 private:
  struct GreedyFace {
    GreedyFace(int axis, BlockPos pos, int u_len, int v_len,
               const BlockID* cache_ptr, BlockID curr, BlockID back)
        : cache_ptr_{cache_ptr},
          starting_pos_{pos},
          axis_{axis},
          u_len_{u_len},
          v_len_{v_len},
          curr_{curr},
          back_{back} {}

    const BlockID* cache_ptr_;
    BlockPos starting_pos_;
    int axis_;
    int u_len_;
    int v_len_;
    BlockID curr_;
    BlockID back_;
  };

  // Used to generate the cache
  void GenerateCache();

  // Generates all the faces and puts them in the cache
  void GenerateFaceCollection();

  // Fluid mesh
  void GenerateFluidMesh();

  // Check if the player can see the mesh
  bool IsFaceVisible(const Cuboid& cube, int side,
                     const BlockID* restrict cache);

  // Add faces to the mesh
  void AddFaceToMesh(const Cuboid& cube, int side, bool allow_ao, BlockPos pos,
                     int u_size, int v_size, const BlockID* restrict cache_ptr);

  void GetAO(int side, const BlockID* restrict cache_ptr, float* ao_m);

  std::pair<int, int> GetLightDirectional(BlockPos pos, int direction);

  static constexpr uint64_t kBufferStepSize = 4096;
  static constexpr int kCacheDim1D = kChunkDim + 2;
  static constexpr int kCacheDim2D = kCacheDim1D * kCacheDim1D;
  static constexpr int kCacheDim3D = kCacheDim2D * kCacheDim1D;

  static constexpr int kCacheStrideX = kCacheDim2D;
  static constexpr int kCacheStrideY = kCacheDim1D;  // 18 * 18 = 324
  static constexpr int kCacheStrideZ = 1;            // 18
  static constexpr int kCacheStride[3]{kCacheStrideX, kCacheStrideY,
                                       kCacheStrideZ};

  static constexpr int kChunkStrideX = kChunkDim * kChunkDim;
  static constexpr int kChunkStrideY = kChunkDim;
  static constexpr int kChunkStrideZ = 1;
  static constexpr int kChunkStride[3]{kChunkStrideX, kChunkStrideY,
                                       kChunkStrideZ};
  std::array<BlockID, kCacheDim3D> chunk_cache_;
  Chunk* chunk_;
};
}  // namespace Mesh
