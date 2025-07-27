// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/Mesh/ChunkMesh.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"
#include "Level/Block/Type/Fluid.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/Palette.h"
#include "Level/Light/LightStorage.h"
#include "RenderEngine/BlockModel/BlockModelManager.h"
#include "RenderEngine/BlockModel/BlockModels.h"
#include "RenderEngine/ChunkRender/Mesh/BlockVertexFormat.h"
#include "Utils/Timer/Timer.h"

Mesh::ChunkMeshData::ChunkMeshData(GameContext& game_context)
    : game_context_{game_context} {
  chunk_cache_.fill(game_context_.blocks_->AIR);
}

Mesh::ChunkMeshData::~ChunkMeshData() = default;

void Mesh::ChunkMeshData::Reset() {
  if (vertices_buffer_.size() != kBufferStepSize) {
    vertices_buffer_.resize(kBufferStepSize);
  }

  if (transparent_vertices_buffer_.size() != kBufferStepSize) {
    transparent_vertices_buffer_.resize(kBufferStepSize);
  }

  transparent_face_count_ = 0;
  solid_face_count_ = 0;

  chunk_cache_.fill(game_context_.blocks_->AIR);
}

void Mesh::ChunkMeshData::GenerateCache() {
  BlockPos pos{0, 0, 0};

  std::array<BlockID, kChunkSize3D> chunk_data =
      chunk_->GetPalette().UnpackAll();

  const BlockID* restrict src = &chunk_data[0];
  BlockID* restrict dst =
      &chunk_cache_[kCacheStrideX + kCacheStrideY + kCacheStrideZ];

  size_t src_x = 0;
  size_t dst_x = 0;
  for (int x = 0; x < kChunkDim; ++x) {
    size_t src_y = src_x;
    size_t dst_y = dst_x;
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

    std::optional<ChunkContainer*> neighbor = chunk_->GetNeighbor(side);

    if (!neighbor.has_value()) {
      continue;
    }

    std::array<BlockID, kChunkSize2D> slice_data =
        neighbor.value()->GetPalette().UnpackSlice(axis, direction * 15);

    int axis_u = (axis + 1) % 3;
    int axis_v = (axis + 2) % 3;

    // Cache friendly access pattern for y-axis (faster)
    if (side.GetAxis() == Directions<ChunkPos>::kYAxis) {
      std::swap(axis_u, axis_v);
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
  const BlockModelManager& models = *game_context_.block_model_manager_.get();

  std::array<uint8_t, 1024> face_visibility;
  std::array<uint8_t, 1024> face_visibility_back;

  std::array<uint8_t, kChunkSize2D> used_block{};

  static constexpr int kChunkStrideU = kChunkDim;
  static constexpr int kChunkStrideV = 1;

  for (int axis = 0; axis < 3; axis++) {
    int axis_u = (axis + 1) % 3;
    int axis_v = (axis + 2) % 3;

    if (axis == Directions<BlockPos>::kYAxis) {  // More cache friendly access
                                                 // pattern for y-axis
      std::swap(axis_u, axis_v);
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

          const BlockModel& curr_model = models.GetBlockModel(curr_block);
          const BlockModel& back_model = models.GetBlockModel(back_block);

          bool blank_curr_model =
              !curr_model.is_initialized_ || slice == kChunkDim;
          bool blank_back_model = !back_model.is_initialized_ || slice == 0;

          // Check if it is visible from the back and front
          if (!blank_curr_model) {
            for (size_t i = 0; i < curr_model.elements_.size(); ++i) {
              face_visibility[i] = 0;
              const Cuboid& element = curr_model.elements_[i];

              if (element.faces_[back_side].reference_texture_.size() == 0)
                continue;

              if (element.faces_[back_side].cull_face_ != -1) {
                if (!IsFaceVisible(element,
                                   element.faces_[back_side].cull_face_,
                                   cache_ptr))
                  continue;
              }
              face_visibility[i] = 1;
            }
          }
          // Check if it is visible from the back and front
          if (!blank_back_model) {
            for (size_t i = 0; i < back_model.elements_.size(); ++i) {
              face_visibility_back[i] = 0;
              const Cuboid& element = back_model.elements_[i];

              if (element.faces_[front_side].reference_texture_.length() == 0)
                continue;

              if (element.faces_[front_side].cull_face_ != -1) {
                if (!IsFaceVisible(element,
                                   element.faces_[front_side].cull_face_,
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
            for (size_t i = 0; i < curr_model.elements_.size(); i++) {
              if (!face_visibility[i]) continue;
              const Cuboid& element = curr_model.elements_[i];
              AddFaceToMesh(element, back_side, curr_model.ambient_occlusion_,
                            q_pos, u_length, v_length, cache_ptr);
            }
          }
          --q_pos[axis];
          if (!blank_back_model) [[likely]] {
            for (size_t i = 0; i < back_model.elements_.size(); i++) {
              if (!face_visibility_back[i]) continue;
              const Cuboid& element = back_model.elements_[i];
              AddFaceToMesh(element, front_side, back_model.ambient_occlusion_,
                            q_pos, u_length, v_length,
                            cache_ptr - stride_slice);
            }
          }

          v += v_length;
          v_ptr += v_length * stride_v;
          used_ptr += v_length;
        }
        u_ptr += stride_u;
      }
      slice_ptr += stride_slice;
    }
  }
}

void Mesh::ChunkMeshData::AddFaceToMesh(const Cuboid& cube, int side,
                                        bool allow_ao, BlockPos pos, int u_size,
                                        int v_size,
                                        const BlockID* restrict cache_ptr) {
  // size_t t0 = __rdtsc();

  //  First get some of the general data
  const int direction = side & 1;
  const int axis = side >> 1;
  const int axis_u = (axis + 1) % 3;
  const int axis_v = (axis + 2) % 3;
  const int new_face_count = u_size * v_size;

  const int stride_u = kCacheStride[axis_u];
  const int stride_v = kCacheStride[axis_v];

  const BlockFace& face = cube.faces_[side];
  glm::vec3 from = cube.from_;
  glm::vec3 to = cube.to_;

  auto CreateVertex = [&](BlockVertexFormat& v, const glm::vec3& tint_color,
                          const glm::ivec2& uv, float ao_multiplier,
                          int sky_light, int block_light) {
    glm::vec4 color(tint_color * ao_multiplier, 1.0f);
    // Assuming BlockVertexFormat::Set now takes a vec4 for color
    v.SetColor(static_cast<int>(color.r * 255), static_cast<int>(color.g * 255),
               static_cast<int>(color.b * 255), 255);
    v.SetUV(face.texture_id_, uv.x, uv.y);
    v.SetLight(sky_light, block_light);
  };

  if (axis == Directions<BlockPos>::kYAxis) {
    std::swap(u_size, v_size);
  }

  std::vector<BlockVertexFormat>& vertex_buffer =
      face.partially_transparent_pixel_ ? transparent_vertices_buffer_
                                        : vertices_buffer_;

  size_t& face_count = face.partially_transparent_pixel_
                           ? transparent_face_count_
                           : solid_face_count_;

  size_t vertex_index = face_count * 6;

  // Resize buffer if it is soo too small
  while (vertex_buffer.size() <= (face_count + new_face_count) * 6) {
    vertex_buffer.resize(vertex_buffer.size() + kBufferStepSize);
  }

  BlockVertexFormat* restrict vertex_dst = vertex_buffer.data() + vertex_index;

  // Now calculate the AO, lighting, colors
  BlockVertexFormat v_00, v_10, v_11, v_01;
  float ao_multi[4] = {1.0f};
  glm::vec3 color{1.0f};
  if (face.tint_index_ != -1) {
    color = glm::vec3(93.0f / 255.0f, 200.0f / 255.0f, 62.0f / 255.0f);
  }

  glm::ivec2 uv_00 = face.uv_coord_00;
  glm::ivec2 uv_01 = face.uv_coord_10;
  glm::ivec2 uv_10 = face.uv_coord_01;
  glm::ivec2 uv_11 = face.uv_coord_11;

  BlockPos offset_pos = pos;
  const BlockID* u_ptr = cache_ptr;
  for (float u = pos[axis_u]; u < pos[axis_u] + u_size; ++u, u_ptr += stride_u) {
    const BlockID* v_ptr = u_ptr;
    for (float v = pos[axis_v]; v < pos[axis_v] + v_size;
         ++v, v_ptr += stride_v) {
      offset_pos[axis_u] = u;
      offset_pos[axis_v] = v;

      if (allow_ao) {
        GetAO(side, v_ptr, ao_multi);
      }

      float base_slice = offset_pos[axis];

      if (direction) {
        base_slice += from[axis];
      } else {
        base_slice += to[axis];
      }

      const auto [sky_light, block_light] =
          GetLightDirectional(offset_pos, side);

      CreateVertex(v_00, color, uv_00, ao_multi[0b00], sky_light, block_light);
      CreateVertex(v_01, color, uv_01, ao_multi[0b01], sky_light, block_light);
      CreateVertex(v_10, color, uv_10, ao_multi[0b10], sky_light, block_light);
      CreateVertex(v_11, color, uv_11, ao_multi[0b11], sky_light, block_light);

      // Now work on positions
      // Set vertices
      v_00.pos_[axis] = base_slice;
      v_00.pos_[axis_u] = u + from[axis_u];
      v_00.pos_[axis_v] = v + from[axis_v];

      v_01.pos_[axis] = base_slice;
      v_01.pos_[axis_u] = u + from[axis_u];
      v_01.pos_[axis_v] = v + to[axis_v];

      v_10.pos_[axis] = base_slice;
      v_10.pos_[axis_u] = u + to[axis_u];
      v_10.pos_[axis_v] = v + from[axis_v];

      v_11.pos_[axis] = base_slice;
      v_11.pos_[axis_u] = u + to[axis_u];
      v_11.pos_[axis_v] = v + to[axis_v];

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
        std::swap(vertex_dst[1], vertex_dst[2]);
        std::swap(vertex_dst[4], vertex_dst[5]);
      }
      ++face_count;
      vertex_dst += 6;
    }
  }

  // size_t t1 = __rdtsc();
  // add_face_clock_ += t1 - t0;
  // ++add_face_call_count_;
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
  BlockID air = game_context_.blocks_->AIR;
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
bool Mesh::ChunkMeshData::IsFaceVisible(const Cuboid& cube, int side,
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

  const BlockModel& model =
      game_context_.block_model_manager_->GetBlockModel(*cache);

  if (!model.is_initialized_) return true;

  for (size_t i = 0; i < model.elements_.size(); ++i) {
    const Cuboid& element = model.elements_[i];
    if (element.faces_[opposite_side].cull_face_ !=
            opposite_side ||  // TODO(hiheyok): Replace and with OR  later
                              // after figuring out fluid rendering
        element.faces_[opposite_side].can_cull) {
      continue;
    }

    if (side & 1) {  // if the  block arent touching
      if (element.to_[axis] < 1.0 || element.from_[axis] > 0) {
        continue;
      }
    }

    // Check if the faces aren't overlapping

    if (element.from_[axis_u] <= cube.from_[axis_u] &&
        element.to_[axis_u] >= cube.to_[axis_u] &&
        element.from_[axis_v] <= cube.from_[axis_v] &&
        element.to_[axis_v] >= cube.to_[axis_v]) {
      return false;
    }
  }
  return true;
}

std::pair<int, int> Mesh::ChunkMeshData::GetLightDirectional(BlockPos pos,
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
