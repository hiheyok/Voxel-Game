// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/Mesh/ChunkMesh.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#include "Assets/AssetManager.h"
#include "Assets/Types/Models/Data/RenderableModel.h"
#include "Assets/Types/Models/Managers/BlockModelManager.h"
#include "Core/GameContext/GameContext.h"
#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"
#include "Level/Block/Type/Fluid.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/Palette.h"
#include "Level/Light/LightStorage.h"
#include "RenderEngine/ChunkRender/Mesh/BlockVertexFormat.h"
#include "Utils/Timer/Timer.h"

using std::array;
using std::optional;
using std::pair;
using std::swap;
using std::vector;

Mesh::ChunkMeshData::ChunkMeshData(GameContext& context) : context_{context} {
  chunk_cache_.fill(context_.blocks_->AIR);
}

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
}

void Mesh::ChunkMeshData::GenerateCache() {
  array<BlockID, kChunkSize3D> chunk_data = chunk_->GetPalette().UnpackAll();

  const BlockID* restrict src = &chunk_data[0];
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

  for (auto side : Directions<ChunkPos>()) {
    int axis = side.GetAxis();
    int direction = side.GetDirection() & 0b1;

    optional<ChunkContainer*> neighbor = chunk_->GetNeighbor(side);

    if (!neighbor.has_value()) {
      continue;
    }

    int side_idx = direction * (kChunkDim - 1);

    array<BlockID, kChunkSize2D> slice_data =
        neighbor.value()->GetPalette().UnpackSlice(axis, side_idx);

    int axis_u = (axis + 1) % 3;
    int axis_v = (axis + 2) % 3;

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

  array<uint8_t, 1024> face_visibility;
  array<uint8_t, 1024> face_visibility_back;
  array<uint8_t, kChunkSize2D> used_block{};

  static constexpr int kChunkStrideU = kChunkDim;
  static constexpr int kChunkStrideV = 1;

  for (int axis = 0; axis < 3; axis++) {
    int axis_u = (axis + 1) % 3;
    int axis_v = (axis + 2) % 3;

    if (axis == kYAxis) {  // More cache friendly access
                           // pattern for y-axis
      swap(axis_u, axis_v);
    }

    const int stride_slice = kCacheStride[axis];
    const int stride_u = kCacheStride[axis_u];
    const int stride_v = kCacheStride[axis_v];

    const int front_side = axis * 2;
    const int back_side = axis * 2 + 1;

    const BlockID* base_ptr =
        &chunk_cache_[kCacheStrideX + kCacheStrideY + kCacheStrideZ];

    const BlockID* slice_ptr = base_ptr;
    for (int slice = 0; slice <= kChunkDim; ++slice) {  // Slice
      used_block.fill(0);
      uint8_t* __restrict used_ptr = used_block.data();
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

          *used_ptr = 0xFF;

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
            for (size_t i = 0; i < curr_elements.size(); ++i) {
              face_visibility[i] = 0;
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
              face_visibility[i] = 1;
            }
          }
          // Check if it is visible from the back and front
          if (!blank_back_model) {
            const auto& back_elements = back_model->GetElements();
            for (size_t i = 0; i < back_elements.size(); ++i) {
              face_visibility_back[i] = 0;
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

              face_visibility_back[i] = 1;
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
            BlockID curr_block_2 = *q_cache_ptr;
            if (curr_block_2 != curr_block) break;
            BlockID back_block_2 = *(q_cache_ptr - stride_slice);
            if (back_block_2 != back_block) break;

            *q_used_ptr = 0xFFU;
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
          /*
           * Center wont have AO check as it is no sides
           * Do lighting checks when combining faces
           */
          if (!blank_curr_model) [[likely]] {
            const auto& curr_elements = curr_model->GetElements();
            for (size_t i = 0; i < curr_elements.size(); i++) {
              if (!face_visibility[i]) continue;
              const baked_model::Element& element = curr_elements[i];
              AddFaceToMesh(element, back_side,
                            curr_model->CheckAmbientOcclusion(), q_pos,
                            u_length, v_length, cache_ptr);
            }
          }
          --q_pos[axis];
          if (!blank_back_model) [[likely]] {
            const auto& back_elements = back_model->GetElements();
            for (size_t i = 0; i < back_elements.size(); i++) {
              if (!face_visibility_back[i]) continue;
              const baked_model::Element& element = back_elements[i];
              AddFaceToMesh(element, front_side,
                            back_model->CheckAmbientOcclusion(), q_pos,
                            u_length, v_length, cache_ptr - stride_slice);
            }
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
  const int axis_u = (axis + 1) % 3;
  const int axis_v = (axis + 2) % 3;
  const int new_face_count = u_size * v_size;
  bool is_trans = element.faces_[side]->partial_trans_;
  int tint_index = element.faces_[side]->tint_index_;
  glm::vec2 uv_00 = element.faces_[side]->uv_00_;
  glm::vec2 uv_01 = element.faces_[side]->uv_01_;
  glm::vec2 uv_10 = element.faces_[side]->uv_10_;
  glm::vec2 uv_11 = element.faces_[side]->uv_11_;
  vector<BlockVertexFormat>& buffer = is_trans ? trans_buffer_ : solid_buffer_;
  size_t& face_count = is_trans ? trans_face_count_ : solid_face_count_;
  size_t vertex_index = face_count * 6;
  face_count += new_face_count;
  // Resize buffer if it is soo too small
  while (buffer.size() <= face_count * 6) {
    buffer.resize(buffer.size() + kBufferStepSize);
  }

  BlockVertexFormat* vertex_dst = buffer.data() + vertex_index;

  const int stride_u = kCacheStride[axis_u];
  const int stride_v = kCacheStride[axis_v];

  if (axis == kYAxis) {
    swap(u_size, v_size);
  }

  auto CreateVertex = [&](BlockVertexFormat& v, const glm::vec3& tint_color,
                          const glm::vec2& uv, float ao_multiplier,
                          int sky_light, int block_light) {
    glm::vec4 color(tint_color * ao_multiplier, 1.0f);
    // Assuming BlockVertexFormat::Set now takes a vec4 for color
    v.SetColor(static_cast<int>(color.r * 255), static_cast<int>(color.g * 255),
               static_cast<int>(color.b * 255), 255);
    v.SetUV(uv.x, uv.y);
    v.SetLight(sky_light, block_light);
  };

  // Now calculate the AO, lighting, colors, vertices
  int slice_mask = (1 << axis) * (~side & 1);
  int u_mask = 1 << axis_u;
  int v_mask = 1 << axis_v;

  BlockVertexFormat v_00, v_10, v_11, v_01;
  v_00.pos_ = element.corners_[slice_mask];
  v_01.pos_ = element.corners_[slice_mask | v_mask];
  v_10.pos_ = element.corners_[slice_mask | u_mask];
  v_11.pos_ = element.corners_[slice_mask | u_mask | v_mask];

  float ao_multi[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec3 color{1.0f};
  if (tint_index != -1) {
    color = {93.0f / 255, 200.0f / 255, 62.0f / 255};
  }

  BlockPos offset_pos = pos;

  v_00.pos_ += glm::vec3{pos.x, pos.y, pos.z};
  v_01.pos_ += glm::vec3{pos.x, pos.y, pos.z};
  v_10.pos_ += glm::vec3{pos.x, pos.y, pos.z};
  v_11.pos_ += glm::vec3{pos.x, pos.y, pos.z};

  const int u1 = pos[axis_u] + u_size;
  const int v1 = pos[axis_v] + v_size;
  const BlockID* u_ptr = cache_ptr;

  for (int u = pos[axis_u]; u < u1; ++u, u_ptr += stride_u) {
    const BlockID* v_ptr = u_ptr;
    for (int v = pos[axis_v]; v < v1; ++v, v_ptr += stride_v) {
      offset_pos[axis_u] = u;
      offset_pos[axis_v] = v;

      if (allow_ao) {
        GetAO(side, v_ptr, ao_multi);
      }

      auto [sky_light, block_light] = GetLightDirectional(offset_pos, side);

      CreateVertex(v_00, color, uv_00, ao_multi[0b00], sky_light, block_light);
      CreateVertex(v_01, color, uv_01, ao_multi[0b01], sky_light, block_light);
      CreateVertex(v_10, color, uv_10, ao_multi[0b10], sky_light, block_light);
      CreateVertex(v_11, color, uv_11, ao_multi[0b11], sky_light, block_light);

      // Now work on positions
      // Set vertices

      if (ao_multi[0] + ao_multi[3] > ao_multi[1] + ao_multi[2]) {
        // Flipped quad
        vertex_dst[0] = v_00;
        vertex_dst[1] = v_10;
        vertex_dst[2] = v_11;

        vertex_dst[3] = v_00;
        vertex_dst[4] = v_11;
        vertex_dst[5] = v_01;
      } else {
        // Normal quad
        vertex_dst[0] = v_00;
        vertex_dst[1] = v_10;
        vertex_dst[2] = v_01;

        vertex_dst[3] = v_01;
        vertex_dst[4] = v_10;
        vertex_dst[5] = v_11;
      }

      // Flip vertices for face culling
      if (direction == 0) {
        swap(vertex_dst[1], vertex_dst[2]);
        swap(vertex_dst[4], vertex_dst[5]);
      }

      v_00.pos_[axis_v]++;
      v_01.pos_[axis_v]++;
      v_10.pos_[axis_v]++;
      v_11.pos_[axis_v]++;

      vertex_dst += 6;
    }

    v_00.pos_[axis_v] -= v_size;
    v_01.pos_[axis_v] -= v_size;
    v_10.pos_[axis_v] -= v_size;
    v_11.pos_[axis_v] -= v_size;

    v_00.pos_[axis_u]++;
    v_01.pos_[axis_u]++;
    v_10.pos_[axis_u]++;
    v_11.pos_[axis_u]++;
  }
}

void Mesh::ChunkMeshData::GetAO(int side, const BlockID* restrict cache_ptr,
                                float* ao_m) {
  constexpr float kTable[4] = {0.40f, 0.60f, 0.80f, 1.00f};
  const int axis = side >> 1;
  const int stride_u = kCacheStride[(axis + 1) % 3];
  const int stride_v = kCacheStride[(axis + 2) % 3];

  const BlockID* restrict p = cache_ptr;
  if ((side & 1) == 0) {
    p += kCacheStride[axis];
  } else {
    p -= kCacheStride[axis];
  }
  BlockID air = context_.blocks_->AIR;
  const int s_u = (p[stride_u] != air);
  const int s_d = (p[-stride_u] != air);
  const int s_r = (p[stride_v] != air);
  const int s_l = (p[-stride_v] != air);

  int ao_00 = 3 - s_d - s_l;
  int ao_01 = 3 - s_d - s_r;
  int ao_10 = 3 - s_u - s_l;
  int ao_11 = 3 - s_u - s_r;

  const BlockID* q = p - stride_u;
  ao_00 -= ((ao_00 == 1u) | (*(q - stride_v) != air));
  ao_01 -= ((ao_01 == 1u) | (*(q + stride_v) != air));
  q += 2 * stride_u;
  ao_10 -= ((ao_10 == 1u) | (*(q - stride_v) != air));
  ao_11 -= ((ao_11 == 1u) | (*(q + stride_v) != air));
  ao_m[0b00] = kTable[ao_00];
  ao_m[0b01] = kTable[ao_01];
  ao_m[0b10] = kTable[ao_10];
  ao_m[0b11] = kTable[ao_11];
}

// Checks if a block side is visible to the player
bool Mesh::ChunkMeshData::IsFaceVisible(const baked_model::Element& element,
                                        int side,
                                        const BlockID* restrict cache) {
  const int axis = (side >> 1);  // Get side
  const int axis_u = (axis + 1) % 3;
  const int axis_v = (axis + 2) % 3;
  const int opposite_side = side ^ 1;  // Flip the leading bit

  if ((side & 1) == 0) {
    cache += kCacheStride[axis];
  } else {
    cache -= kCacheStride[axis];
  }

  const BlockModelManager::ModelList& model_list = model_manager_->GetModels();
  const AssetHandle<RenderableModel>& neigh_model = model_list[*cache];

  if (!neigh_model.HasValue()) return true;
  auto neigh_elements = neigh_model->GetElements();

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

pair<int, int> Mesh::ChunkMeshData::GetLightDirectional(BlockPos pos,
                                                        int side) {
  const int axis = side >> 1;

  pos.IncrementSide(side, 1);
  ChunkContainer* chunk = chunk_;
  if (0 > pos[axis] || kChunkDim <= pos[axis]) {
    auto neigh = chunk->GetNeighbor(side);
    if (!neigh.has_value()) return {0, 0};
    chunk = neigh.value();
  }

  return {chunk->sky_light_->GetLighting(pos),
          chunk->block_light_->GetLighting(pos)};
}
