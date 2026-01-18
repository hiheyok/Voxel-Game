// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/Mesh/ChunkMesh.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <optional>
#include <utility>
#include <vector>

#include "Assets/AssetHandle.h"
#include "Assets/AssetManager.h"
#include "Assets/Types/Models/Data/RenderableModel.h"
#include "Assets/Types/Models/Managers/BlockModelManager.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/ChunkData.h"
#include "Level/Chunk/Palette.h"
#include "Level/Light/LightStorage.h"
#include "RenderEngine/ChunkRender/Mesh/BlockVertexFormat.h"
#include "Utils/Timer/Timer.h"

using glm::vec2;
using glm::vec3;
using std::array;
using std::optional;
using std::pair;
using std::swap;
using std::vector;

namespace {

static constexpr uint8_t kTriIdx[4][6] = {
    /* case 0: !use_flipped, !flip_winding */ {0, 2, 1, 1, 2, 3},
    /* case 1: !use_flipped,  flip_winding */ {0, 1, 2, 1, 3, 2},
    /* case 2:  use_flipped, !flip_winding */ {0, 2, 3, 0, 3, 1},
    /* case 3:  use_flipped,  flip_winding */ {0, 3, 2, 0, 1, 3},
};

constexpr array<array<uint8_t, 256>, 4> MakeMulAO() {
  array<array<uint8_t, 256>, 4> t{};
  for (uint32_t c = 0; c < 256; ++c) {
    t[0][c] = c * 2 / 5;                // ao=2
    t[1][c] = c * 3 / 5;                // ao=3
    t[2][c] = c * 4 / 5;                // ao=4
    t[3][c] = static_cast<uint8_t>(c);  // ao=5
  }
  return t;
};

static constexpr int kAxisU[3] = {1, 2, 0};
static constexpr int kAxisV[3] = {2, 0, 1};

alignas(64) inline constexpr auto kMulAO = MakeMulAO();

static inline uint8_t MulColorByte(uint8_t clr, uint8_t ao) noexcept {
  return kMulAO[ao][clr];  // ao in {2,3,4,5}
}

static inline void StoreRGB(BlockVertexFormat& out, uint8_t r, uint8_t g,
                            uint8_t b) {
  out.rgba_ =
      uint32_t(r) | (uint32_t(g) << 8) | (uint32_t(b) << 16) | 0xFF000000u;
}

static inline uint32_t PackRGBMulAO(const uint8_t clr[3], uint8_t ao) noexcept {
  const uint8_t r = MulColorByte(clr[0], ao);
  const uint8_t g = MulColorByte(clr[1], ao);
  const uint8_t b = MulColorByte(clr[2], ao);
  return uint32_t(r) | (uint32_t(g) << 8) | (uint32_t(b) << 16) | 0xFF000000u;
}

}  // namespace

Mesh::ChunkMeshData::ChunkMeshData(GameContext& context) : context_{context} {
  chunk_cache_.fill(context_.blocks_->AIR);
}

// Static sentinel LightStorage - always returns 0 light values
const LightStorage Mesh::ChunkMeshData::kNullLight_;

Mesh::ChunkMeshData::~ChunkMeshData() = default;

void Mesh::ChunkMeshData::Reset() {
  if (solid_buffer_.size() != kBufferStepSize) {
    solid_buffer_.resize(kBufferStepSize);
  }

  if (trans_buffer_.size() != kBufferStepSize) {
    trans_buffer_.resize(kBufferStepSize);
  }

  trans_face_count_ = 0;
  solid_face_count_ = 0;

  chunk_cache_.fill(context_.blocks_->AIR);
  cached_sky_light_.fill(&kNullLight_);
  cached_block_light_.fill(&kNullLight_);
}

void Mesh::ChunkMeshData::GenerateCache() {
  // Cache LightStorage pointers for branchless light lookups
  // Index 0 = self, indices 1-6 = neighbors (side+1)
  // Use kNullLight_ sentinel for missing neighbors (always returns 0)
  cached_sky_light_[0] = chunk_->sky_light_.get();
  cached_block_light_[0] = chunk_->block_light_.get();
  for (int i = 0; i < 6; ++i) {
    ChunkContainer* neighbor = chunk_->GetNeighborRaw(i);
    if (!neighbor) continue;
    cached_sky_light_[i + 1] = neighbor->sky_light_.get();
    cached_block_light_[i + 1] = neighbor->block_light_.get();
  }

  unpack_scratch_ = chunk_->GetPalette().UnpackAll();

  const BlockID* restrict src = &unpack_scratch_[0];
  BlockID* restrict dst =
      &chunk_cache_[kCacheStrideX + kCacheStrideY + kCacheStrideZ];

  size_t src_x = 0;
  size_t dst_x = 0;
  size_t src_y;
  size_t dst_y;
  for (int x = 0; x < kChunkDim; ++x) {
    src_y = src_x;
    dst_y = dst_x;
    for (int y = 0; y < kChunkDim; ++y) {
      memcpy(dst + dst_y, src + src_y, kChunkDim * sizeof(BlockID));

      src_y += kChunkDim;
      dst_y += kCacheDim1D;
    }
    src_x += kChunkSize2D;
    dst_x += kCacheDim2D;
  }

  // Use static reference for optimized iteration
  // Fetch blocks from orthogonal sides
  static constexpr auto& kDirs = Directions<ChunkPos>::kDirections;
  for (const auto& side : kDirs) {
    int axis = side.GetAxis();
    int direction = side.GetDirection() & 0b1;

    optional<ChunkContainer*> neighbor = chunk_->GetNeighbor(side);

    if (!neighbor.has_value()) {
      continue;
    }

    int side_idx = direction * (kChunkDim - 1);

    array<BlockID, kChunkSize2D> slice_data =
        neighbor.value()->GetPalette().UnpackSlice(axis, side_idx);

    int axis_u = kAxisU[axis];
    int axis_v = kAxisV[axis];

    // Cache friendly access pattern for y-axis (faster)
    if (side.GetAxis() == kYAxis) {
      swap(axis_u, axis_v);
    }

    const int stride_slice = kCacheStride[axis];
    const int stride_u = kCacheStride[axis_u];
    const int stride_v = kCacheStride[axis_v];

    src = slice_data.data();
    dst = &chunk_cache_[stride_slice * (kCacheDim1D - 1) * (direction ^ 1) +
                        stride_u + stride_v];

    size_t src_idx = 0;
    size_t dst_u = 0;
    for (int u = 0; u < kChunkDim; ++u) {
      size_t dst_v = dst_u;
      for (int v = 0; v < kChunkDim; ++v) {
        *(dst + dst_v) = *(src + src_idx++);
        dst_v += stride_v;
      }
      dst_u += stride_u;
    }
  }

  static constexpr size_t kCacheXOffset = kCacheStrideX * (kCacheDim1D - 1);
  static constexpr size_t kCacheYOffset = kCacheStrideY * (kCacheDim1D - 1);
  static constexpr size_t kCacheZOffset = kCacheStrideZ * (kCacheDim1D - 1);
  static constexpr size_t kCacheOffsets[3] = {kCacheXOffset, kCacheYOffset,
                                              kCacheZOffset};

  // Neighbor direction indices: [axis][negative=0/positive=1]
  // X: West=1, East=0  Y: Down=3, Up=2  Z: North=5, South=4
  static constexpr int kNeighborDirs[3][2] = {{1, 0}, {3, 2}, {5, 4}};

  // Cache the 6 orthogonal neighbors (already fetched partially above)
  ChunkContainer* ortho_neighbors[6];
  for (int i = 0; i < 6; ++i) {
    ortho_neighbors[i] = chunk_->GetNeighborRaw(i);
  }

  // Compute diagonal neighbors for edges using cached ortho neighbors
  // Only 12 valid diagonals (where u_axis != v_axis)
  // Indexed as: diag[u_axis * 6 + v_axis * 2 + u_neg_flag * 2 + v_neg_flag]
  // Simpler: flat array of 12 entries
  struct EdgeInfo {
    const ChunkContainer* chunk;
    int axis;     // The varying axis
    int u_axis;   // First fixed axis
    int v_axis;   // Second fixed axis
    int u_local;  // Local coord in u_axis (0 or 15)
    int v_local;  // Local coord in v_axis (0 or 15)
    size_t cache_base;
  };

  EdgeInfo edges[12];
  int edge_count = 0;

  for (int axis = 0; axis < 3; ++axis) {
    const int u_axis = kAxisU[axis];
    const int v_axis = kAxisV[axis];

    for (int u_m = 0; u_m < 2; ++u_m) {
      for (int v_m = 0; v_m < 2; ++v_m) {
        int u_dir = kNeighborDirs[u_axis][u_m];
        int v_dir = kNeighborDirs[v_axis][v_m];

        ChunkContainer* u_neigh = ortho_neighbors[u_dir];
        if (!u_neigh) continue;

        ChunkContainer* diag = u_neigh->GetNeighborRaw(v_dir);
        if (!diag) continue;

        edges[edge_count++] = {diag,
                               axis,
                               u_axis,
                               v_axis,
                               (u_m == 0) ? 15 : 0,
                               (v_m == 0) ? 15 : 0,
                               kCacheOffsets[u_axis] * u_m +
                                   kCacheOffsets[v_axis] * v_m +
                                   kCacheStride[axis]};
      }
    }
  }

  // Prefetch edge chunk palettes before reading
  for (int e = 0; e < edge_count; ++e) {
#if defined(_MSC_VER)
    _mm_prefetch(reinterpret_cast<const char*>(&edges[e].chunk->GetPalette()),
                 _MM_HINT_T0);
#elif defined(__GNUC__) || defined(__clang__)
    __builtin_prefetch(&edges[e].chunk->GetPalette(), 0, 3);
#endif
  }

  // Fetch edge blocks
  for (int e = 0; e < edge_count; ++e) {
    const EdgeInfo& edge = edges[e];
    BlockPos local_pos;
    local_pos[edge.u_axis] = edge.u_local;
    local_pos[edge.v_axis] = edge.v_local;

    for (int t = 0; t < kChunkDim; ++t) {
      local_pos[edge.axis] = t;
      chunk_cache_[edge.cache_base + kCacheStride[edge.axis] * t] =
          edge.chunk->GetBlockUnsafe(local_pos);
    }
  }

  // Fetch the 8 corner blocks
  // Corner chunk is reached via 3 hops from ortho neighbors
  for (int i = 0; i < 8; ++i) {
    int x_m = (i >> 0) & 1;
    int y_m = (i >> 1) & 1;
    int z_m = (i >> 2) & 1;

    // Hop: X neighbor -> Y neighbor -> Z neighbor
    ChunkContainer* x_neigh = ortho_neighbors[kNeighborDirs[0][x_m]];
    if (!x_neigh) continue;

    ChunkContainer* xy_neigh = x_neigh->GetNeighborRaw(kNeighborDirs[1][y_m]);
    if (!xy_neigh) continue;

    ChunkContainer* xyz_neigh = xy_neigh->GetNeighborRaw(kNeighborDirs[2][z_m]);
    if (!xyz_neigh) continue;

    // Prefetch next corner's palette while processing this one
    if (i + 1 < 8) {
      int nx_m = ((i + 1) >> 0) & 1;
      int ny_m = ((i + 1) >> 1) & 1;
      int nz_m = ((i + 1) >> 2) & 1;
      ChunkContainer* nx = ortho_neighbors[kNeighborDirs[0][nx_m]];
      if (nx) {
        ChunkContainer* nxy = nx->GetNeighborRaw(kNeighborDirs[1][ny_m]);
        if (nxy) {
          ChunkContainer* nxyz = nxy->GetNeighborRaw(kNeighborDirs[2][nz_m]);
          if (nxyz) {
#if defined(_MSC_VER)
            _mm_prefetch(reinterpret_cast<const char*>(&nxyz->GetPalette()),
                         _MM_HINT_T0);
#elif defined(__GNUC__) || defined(__clang__)
            __builtin_prefetch(&nxyz->GetPalette(), 0, 3);
#endif
          }
        }
      }
    }

    BlockPos local_pos{x_m == 0 ? 15 : 0, y_m == 0 ? 15 : 0, z_m == 0 ? 15 : 0};
    chunk_cache_[kCacheXOffset * x_m + kCacheYOffset * y_m +
                 kCacheZOffset * z_m] = xyz_neigh->GetBlockUnsafe(local_pos);
  }
}

void Mesh::ChunkMeshData::SetChunk(Chunk* chunk) { chunk_ = chunk; }

void Mesh::ChunkMeshData::GenerateMesh() {
  // Initialize
  if (chunk_ == nullptr) {
    return;
  }
  model_manager_ = &context_.assets_->GetBlockModelManager();
  greedy_time_ = 0;
  add_face_call_count_ = 0;
  add_face_clock_ = 0;
  Timer time;
  GenerateCache();
  cache_time_ = time.GetTimePassed_us();
  GenerateFaceCollection();
  greedy_time_ = time.GetTimePassed_us() - cache_time_;
}

void Mesh::ChunkMeshData::GenerateFaceCollection() {
  const BlockModelManager::ModelList& models = model_manager_->GetModels();

  // Cache air block ID for GetAO (avoids dereference in hot path)
  cached_air_id_ = context_.blocks_->AIR;

  static constexpr int kChunkStrideU = kChunkDim;
  static constexpr int kChunkStrideV = 1;

  for (int axis = 0; axis < 3; axis++) {
    int axis_u = kAxisU[axis];
    int axis_v = kAxisV[axis];

    // Cache friendly access pattern
    if (axis == kYAxis) swap(axis_u, axis_v);

    const int stride_slice = kCacheStride[axis];
    const int stride_u = kCacheStride[axis_u];
    const int stride_v = kCacheStride[axis_v];

    const int front_side = axis * 2;
    const int back_side = axis * 2 + 1;

    const BlockID* base_ptr =
        &chunk_cache_[kCacheStrideX + kCacheStrideY + kCacheStrideZ];

    // Cache the element ptr array
    const baked_model::Element* curr_elems_ptr = nullptr;
    size_t curr_elems_n = 0;
    bool curr_allow_ao = false;

    const baked_model::Element* back_elems_ptr = nullptr;
    size_t back_elems_n = 0;
    bool back_allow_ao = false;

    const BlockID* slice_ptr = base_ptr;
    for (int slice = 0; slice <= kChunkDim; ++slice) {  // Slice
      memset(used_block_.data(), 0, kChunkSize2D);
      uint8_t* __restrict used_ptr = used_block_.data();
      const BlockID* u_ptr = slice_ptr;
      for (int u = 0; u < kChunkDim; ++u) {
        const BlockID* v_ptr = u_ptr;
        for (int v = 0; v < kChunkDim;) {
          if (*used_ptr != 0x00) {
            v += *used_ptr;
            v_ptr += (*used_ptr) * stride_v;
            used_ptr += *used_ptr;
            continue;
          }

          const BlockID* cache_ptr = v_ptr;

          BlockID curr_block = *cache_ptr;
          BlockID back_block = *(cache_ptr - stride_slice);

          const auto& curr_model = models[curr_block];
          const auto& back_model = models[back_block];

          bool blank_curr_model = !curr_model.HasValue() || slice == kChunkDim;
          bool blank_back_model = !back_model.HasValue() || slice == 0;

          // Check if it is visible from the back and front
          if (!blank_curr_model) {
            const auto& curr_elements = curr_model->GetElements();
            curr_elems_ptr = curr_elements.data();
            curr_elems_n = curr_elements.size();
            curr_allow_ao = curr_model->CheckAmbientOcclusion();

            for (size_t i = 0; i < curr_elems_n; ++i) {
              const baked_model::Element& element = curr_elements[i];

              if (!element.faces_[back_side].has_value()) {
                continue;
              }

              if (element.faces_[back_side]->cull_face_ != -1) {
                if (!IsFaceVisible(element,
                                   element.faces_[back_side]->cull_face_,
                                   cache_ptr))
                  continue;
              }
              visible_idx_curr_[visible_idx_curr_n_++] = i;
            }
          }
          // Check if it is visible from the back and front
          if (!blank_back_model) {
            const auto& back_elements = back_model->GetElements();
            back_elems_ptr = back_elements.data();
            back_elems_n = back_elements.size();
            back_allow_ao = back_model->CheckAmbientOcclusion();

            for (size_t i = 0; i < back_elems_n; ++i) {
              const baked_model::Element& element = back_elements[i];

              if (!element.faces_[front_side].has_value()) {
                continue;
              }

              if (element.faces_[front_side]->cull_face_ != -1) {
                if (!IsFaceVisible(element,
                                   element.faces_[front_side]->cull_face_,
                                   cache_ptr - stride_slice))
                  continue;
              }

              visible_idx_back_[visible_idx_back_n_++] = i;
            }
          }

          // Spread

          int u_length = 1;
          int v_length = 1;

          const BlockID* q_cache_ptr = cache_ptr;
          uint8_t* q_used_ptr = used_ptr;

          for (int v_ex = v + 1; v_ex < kChunkDim; ++v_ex) {
            q_cache_ptr += stride_v;
            q_used_ptr += kChunkStrideV;
            // Check if they are the same
            if (*q_cache_ptr != curr_block) break;
            if (*(q_cache_ptr - stride_slice) != back_block) break;

            v_length++;
          }

          q_cache_ptr = cache_ptr + stride_u;
          q_used_ptr = used_ptr;
          for (int u_ex = u + 1; u_ex < kChunkDim;
               ++u_ex, q_cache_ptr += stride_u) {
            const BlockID* q_ptr_v_end = q_cache_ptr + stride_v * v_length;
            const BlockID* q_ptr_v = q_cache_ptr;
            for (; q_ptr_v < q_ptr_v_end; q_ptr_v += stride_v) {
              BlockID curr_block_2 = *q_ptr_v;
              BlockID back_block_2 = *(q_ptr_v - stride_slice);
              if (curr_block_2 != curr_block || back_block_2 != back_block)
                  [[unlikely]] {
                goto skip;
              }
            }

            q_used_ptr += kChunkStrideU;
            memset(q_used_ptr, v_length, v_length);
            ++u_length;
          }
        skip:

          BlockPos q_pos;
          q_pos[axis] = slice;
          q_pos[axis_u] = u;
          q_pos[axis_v] = v;

          // Memorize & Add Faces
          while (visible_idx_curr_n_ > 0) {
            const baked_model::Element& element =
                curr_elems_ptr[visible_idx_curr_[--visible_idx_curr_n_]];
            AddFaceToMesh(element, back_side, curr_allow_ao, q_pos, u_length,
                          v_length, cache_ptr);
          }
          --q_pos[axis];
          while (visible_idx_back_n_ > 0) {
            const baked_model::Element& element =
                back_elems_ptr[visible_idx_back_[--visible_idx_back_n_]];
            AddFaceToMesh(element, front_side, back_allow_ao, q_pos, u_length,
                          v_length, cache_ptr - stride_slice);
          }

          v += v_length;
#if defined(__GNUC__) || defined(__clang__)
          __builtin_prefetch(v_ptr + stride_v * 8, 0, 1);
#endif
          v_ptr += v_length * stride_v;
          used_ptr += v_length;
        }
        u_ptr += stride_u;
      }
      slice_ptr += stride_slice;
    }
  }
}

void Mesh::ChunkMeshData::AddFaceToMesh(const baked_model::Element& element,
                                        int side, bool allow_ao, BlockPos pos,
                                        int u_size, int v_size,
                                        const BlockID* restrict cache_ptr) {
  //  First get some of the general data
  const int direction = side & 1;
  const int axis = side >> 1;
  const int axis_u = kAxisU[axis];
  const int axis_v = kAxisV[axis];
  const int new_face_count = u_size * v_size;

  if (axis == kYAxis) {
    swap(u_size, v_size);
  }
  const auto& face = *element.faces_[side];

  // Get the face data
  bool is_trans = face.partial_trans_;
  int tint_index = face.tint_index_;
  vec2 uv[4] = {face.uv_00_, face.uv_01_, face.uv_10_, face.uv_11_};

  // Manage buffer
  vector<BlockVertexFormat>& buffer = is_trans ? trans_buffer_ : solid_buffer_;
  size_t& face_count = is_trans ? trans_face_count_ : solid_face_count_;
  size_t vertex_index = face_count * 6;
  face_count += new_face_count;
  // Resize buffer if it is soo too small
  while (buffer.size() <= face_count * 6) {
    buffer.resize(buffer.size() + kBufferStepSize);
  }

  BlockVertexFormat* vertex_dst = buffer.data() + vertex_index;

  BlockPos light_pos = pos;
  light_pos.IncrementSide(side, 1);
  const bool out_of_bounds =
      (light_pos[axis] < 0) | (light_pos[axis] >= kChunkDim);
  const int light_idx = out_of_bounds ? (side + 1) : 0;
  const LightStorage& sky_ls = *cached_sky_light_[light_idx];
  const LightStorage& blk_ls = *cached_block_light_[light_idx];
  light_pos = light_pos.GetLocalPos();

  const int stride_u = kCacheStride[axis_u];
  const int stride_v = kCacheStride[axis_v];

  // Pre-compute light index base and strides for inline access
  // BlockPos::GetIndex() = kChunkSize2D * (x & mask) | kChunkDim * (y & mask) |
  // (z & mask) This is linear in each axis, so we can compute incrementally
  static constexpr int kLightStride[3] = {kChunkSize2D, kChunkDim, 1};
  const int light_stride_u = kLightStride[axis_u];
  const int light_stride_v = kLightStride[axis_v];
  const int base_light_idx =
      light_pos[0] * kChunkSize2D + light_pos[1] * kChunkDim + light_pos[2];

  // Now calculate the AO, lighting, colors, vertices
  int slice_mask = (1 << axis) * (~side & 1);
  int u_mask = 1 << axis_u;
  int v_mask = 1 << axis_v;

  uint8_t clr[3] = {255, 255, 255};
  if (tint_index != -1) {
    clr[0] = 93;
    clr[1] = 200;
    clr[2] = 62;
  }

  const uint32_t rgba_by_ao[4] = {
      PackRGBMulAO(clr, 0),
      PackRGBMulAO(clr, 1),
      PackRGBMulAO(clr, 2),
      PackRGBMulAO(clr, 3),
  };

  BlockPos offset_pos = pos;

  const uint8_t corner_idx[4] = {slice_mask, slice_mask | v_mask,
                                 slice_mask | u_mask,
                                 slice_mask | u_mask | v_mask};

  // AO indices (0..3). Full-bright default.
  uint8_t ao_m[4] = {3, 3, 3, 3};

  const int u0 = pos[axis_u];
  const int v0 = pos[axis_v];
  const int u1 = pos[axis_u] + u_size;
  const int v1 = pos[axis_v] + v_size;

  const BlockID* u_ptr = cache_ptr;
  vec3 base_pos = {float(pos.x), float(pos.y), float(pos.z)};
  for (int u = u0; u < u1; ++u, u_ptr += stride_u) {
    const BlockID* v_ptr = u_ptr;

    int light_idx =
        base_light_idx + (u - u0) * light_stride_u + (v0 - v0) * light_stride_v;

    base_pos[axis_u] = float(u);

    for (int v = v0; v < v1;
         ++v, v_ptr += stride_v, light_idx += light_stride_v) {
      base_pos[axis_v] = float(v);

      if (allow_ao) {
        GetAO(side, v_ptr, ao_m);
      }

      // Inline light access: compute index directly instead of GetLighting()
      const int sky_light = sky_ls.GetLightingByIndex(light_idx);
      const int blk_light = blk_ls.GetLightingByIndex(light_idx);
      const int packed_light = sky_light | (blk_light << 4);

      // Choose quad diagonal and winding using lookup table
      const bool use_flipped = (ao_m[0] + ao_m[3] > ao_m[1] + ao_m[2]);
      const int table_idx = (use_flipped << 1) | (direction == 0);
      const uint8_t* idx = kTriIdx[table_idx];

      for (int i = 0; i < 6; ++i) {
        const int c = idx[i];
        const vec3& corner = element.corners_[corner_idx[c]];

        vertex_dst[i].pos_.x = corner.x + base_pos.x;
        vertex_dst[i].pos_.y = corner.y + base_pos.y;
        vertex_dst[i].pos_.z = corner.z + base_pos.z;
        vertex_dst[i].rgba_ = rgba_by_ao[ao_m[c]];
        vertex_dst[i].u_ = uv[c].x;
        vertex_dst[i].v_ = uv[c].y;
        vertex_dst[i].light_ = packed_light;
      }

      vertex_dst += 6;
    }
  }
}

FORCEINLINE void Mesh::ChunkMeshData::GetAO(int side,
                                            const BlockID* restrict cache_ptr,
                                            uint8_t* ao_m) {
  static constexpr int8_t kSideSign[6] = {1, -1, 1, -1, 1, -1};

  const int axis = side >> 1;
  const int stride_u = kCacheStride[kAxisU[axis]];
  const int stride_v = kCacheStride[kAxisV[axis]];

  // Branchless stride offset: +stride for even side, -stride for odd
  const int stride_axis = kCacheStride[axis];
  const BlockID* restrict p = cache_ptr + stride_axis * kSideSign[side];

  const BlockID air = cached_air_id_;
  const int s_u = (p[stride_u] != air);
  const int s_d = (p[-stride_u] != air);
  const int s_r = (p[stride_v] != air);
  const int s_l = (p[-stride_v] != air);

  const int c_dl = (p[-stride_u - stride_v] != air);
  const int c_dr = (p[-stride_u + stride_v] != air);
  const int c_ul = (p[+stride_u - stride_v] != air);
  const int c_ur = (p[+stride_u + stride_v] != air);

  // (0.2 * ao + 0.4) * 5 = ao + 2
  // + 2 is deleted as thats handled in the lut

  ao_m[0b00] = uint8_t(3 - s_d - s_l - ((s_d & s_l) | c_dl));
  ao_m[0b01] = uint8_t(3 - s_d - s_r - ((s_d & s_r) | c_dr));
  ao_m[0b10] = uint8_t(3 - s_u - s_l - ((s_u & s_l) | c_ul));
  ao_m[0b11] = uint8_t(3 - s_u - s_r - ((s_u & s_r) | c_ur));
}

// Checks if a block side is visible to the player
bool Mesh::ChunkMeshData::IsFaceVisible(const baked_model::Element& element,
                                        int side,
                                        const BlockID* restrict cache) {
  const int axis = (side >> 1);  // Get side
  const int axis_u = kAxisU[axis];
  const int axis_v = kAxisV[axis];
  const int opposite_side = side ^ 1;  // Flip the leading bit

  if (side & 1) {
    cache -= kCacheStride[axis];
  } else {
    cache += kCacheStride[axis];
  }

  const BlockModelManager::ModelList& model_list = model_manager_->GetModels();
  const AssetHandle<RenderableModel>& neigh_model = model_list[*cache];

  if (!neigh_model.HasValue()) return true;
  const auto& neigh_elements = neigh_model->GetElements();

  for (const auto& neigh_element : neigh_elements) {
    if (!neigh_element.faces_[opposite_side].has_value()) {
      continue;
    }
    // TODO(hiheyok): Replace and with OR  later
    // after figuring out fluid rendering
    if (neigh_element.faces_[opposite_side]->cull_face_ != opposite_side ||
        neigh_element.faces_[opposite_side]->partial_trans_ ||
        neigh_element.faces_[opposite_side]->full_trans_) {
      continue;
    }

    if (side & 1) {  // if the block arent touching
      if (neigh_element.to_[axis] < 1.0 || neigh_element.from_[axis] > 0) {
        continue;
      }
    }

    // Check if the faces aren't overlapping

    if (neigh_element.from_[axis_u] <= element.from_[axis_u] &&
        neigh_element.to_[axis_u] >= element.to_[axis_u] &&
        neigh_element.from_[axis_v] <= element.from_[axis_v] &&
        neigh_element.to_[axis_v] >= element.to_[axis_v]) {
      return false;
    }
  }
  return true;
}
