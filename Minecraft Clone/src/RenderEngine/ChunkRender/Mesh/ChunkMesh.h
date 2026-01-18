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
class ChunkContainer;
class GameContext;
class BlockModelManager;
class LightStorage;

struct Cuboid;
struct BlockFace;
struct BlockModel;
struct BlockVertexFormat;

namespace baked_model {
struct Face;
struct Element;
}  // namespace baked_model

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
  std::vector<BlockVertexFormat> solid_buffer_;
  std::vector<BlockVertexFormat> trans_buffer_;
  std::vector<BlockVertexFormat> solid_fluid_vertices_buffer_;
  std::vector<BlockVertexFormat> transparent_fluid_vertices_buffer_;

  size_t trans_face_count_ = 0;
  size_t solid_face_count_ = 0;
  size_t solid_fluid_face_count_ = 0;
  size_t transparent_fluid_face_count_ = 0;

  size_t greedy_time_;
  size_t cache_time_;
  size_t add_face_clock_ = 0;
  size_t add_face_call_count_ = 0;

  bool use_option_ = false;  // for performance debugging purposes

 private:

  // Used to generate the cache
  void GenerateCache();

  // Generates all the faces and puts them in the cache
  void GenerateFaceCollection();

  // Fluid mesh
  void GenerateFluidMesh();

  // Check if the player can see the mesh
  bool IsFaceVisible(const baked_model::Element& element, int side,
                     const BlockID* restrict cache);

  // Add faces to the mesh
  void AddFaceToMesh(const baked_model::Element& element, int side,
                     bool allow_ao, BlockPos pos, int u_size, int v_size,
                     const BlockID* restrict cache_ptr);

  FORCEINLINE void GetAO(int side, const BlockID* restrict cache_ptr,
                         uint8_t* ao_m);

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

  // Visibility arrays as class members to avoid repeated stack allocation
  std::array<uint16_t, 1024> visible_idx_curr_;
  int visible_idx_curr_n_ = 0;
  std::array<uint16_t, 1024> visible_idx_back_;
  int visible_idx_back_n_ = 0;

  std::array<uint8_t, kChunkSize2D> used_block_;

  alignas(64) std::array<BlockID, kCacheDim3D> chunk_cache_;
  alignas(64) std::array<BlockID, kChunkSize3D> unpack_scratch_;
  Chunk* chunk_;
  const BlockModelManager* model_manager_;

  // Cached LightStorage pointers for branchless light lookups
  // Index 0 = self, indices 1-6 = neighbors (side+1)
  std::array<const LightStorage*, 7> cached_sky_light_;
  std::array<const LightStorage*, 7> cached_block_light_;

  // Sentinel LightStorage that always returns 0 (for missing neighbors)
  static const LightStorage kNullLight_;

  // Cached air block ID for fast comparisons in GetAO
  BlockID cached_air_id_;
};
}  // namespace Mesh
