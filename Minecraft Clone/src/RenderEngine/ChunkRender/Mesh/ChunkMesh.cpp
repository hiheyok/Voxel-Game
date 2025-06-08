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

  int chunk_data_index = 0;
  BlockID* base_ptr =
      &chunk_cache_[kCacheStrideX + kCacheStrideY + kCacheStrideZ];

  BlockID* x_ptr = base_ptr;
  for (int x = 0; x < kChunkDim; ++x) {
    BlockID* y_ptr = x_ptr;
    for (int y = 0; y < kChunkDim; ++y) {
      BlockID* z_ptr = y_ptr;
      for (int z = 0; z < kChunkDim; ++z) {
        *z_ptr = chunk_data[chunk_data_index++];
        z_ptr += kCacheStrideZ;
      }
      y_ptr += kCacheStrideY;
    }
    x_ptr += kCacheStrideX;
  }

  for (const auto& side : Directions<ChunkPos>()) {
    int axis = side.GetAxis();
    int direction = side.GetDirection() & 0b1;

    pos[axis] = direction * 15;

    std::optional<ChunkContainer*> neighbor = chunk_->GetNeighbor(side);

    if (!neighbor.has_value()) {
      continue;
    }

    int axis_u = (axis + 1) % 3;
    int axis_v = (axis + 2) % 3;

    // Cache friendly access pattern for y-axis (faster)
    if (side.GetAxis() == Directions<ChunkPos>::kYAxis) {
      std::swap(axis_u, axis_v);
    }

    const int stride_slice = kCacheStride[axis];
    const int stride_u = kCacheStride[axis_u];
    const int stride_v = kCacheStride[axis_v];

    base_ptr = &chunk_cache_[stride_slice * (17 - 17 * direction) + stride_u +
                             stride_v];
    BlockID* u_ptr = base_ptr;
    for (pos[axis_u] = 0; pos[axis_u] < kChunkDim; pos[axis_u]++) {
      BlockID* v_ptr = u_ptr;
      for (pos[axis_v] = 0; pos[axis_v] < kChunkDim; pos[axis_v]++) {
        BlockID block = neighbor.value()->GetBlockUnsafe(pos);
        *v_ptr = block;
        v_ptr += stride_v;
      }
      u_ptr += stride_u;
    }
  }
}

void Mesh::ChunkMeshData::SetChunk(Chunk* pChunk) { chunk_ = pChunk; }

void Mesh::ChunkMeshData::GenerateMesh() {
  // Initialize
  if (chunk_ == nullptr) {
    return;
  }
  greedy_time_ = 0;
  Timer time;
  GenerateCache();
  cache_time_ = time.GetTimePassed_μs();
  GenerateFaceCollection();
  greedy_time_ = time.GetTimePassed_μs() - cache_time_;
}

void Mesh::ChunkMeshData::GenerateFaceCollection() {
  std::array<uint8_t, 4096> face_visibility;
  std::array<uint8_t, 4096> face_visibility_back;
  std::array<uint8_t, kChunkSize2D> used_block{};

  static constexpr int chunk_stride_u = kChunkDim;
  static constexpr int chunk_stride_v = 1;

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

    BlockPos pos{0, 0, 0};

    const BlockID* base_ptr =
        &chunk_cache_[kCacheStrideX + kCacheStrideY + kCacheStrideZ];

    const BlockID* slice_ptr = base_ptr;
    for (pos[axis] = 0; pos[axis] <= kChunkDim; ++pos[axis]) {  // Slice
      used_block.fill(0);
      uint8_t* used_ptr = &used_block[0];
      const BlockID* u_ptr = slice_ptr;
      for (pos[axis_u] = 0; pos[axis_u] < kChunkDim; ++pos[axis_u]) {
        const BlockID* v_ptr = u_ptr;
        for (pos[axis_v] = 0; pos[axis_v] < kChunkDim; ++pos[axis_v]) {
          if (*used_ptr != 0x00) {
            pos[axis_v] += *used_ptr-1;
            v_ptr += (*used_ptr) * stride_v;
            used_ptr += *used_ptr;
            continue;
          }

          *used_ptr = 0xFF;

          const BlockID* cache_ptr = v_ptr;

          BlockID curr_block = *cache_ptr;
          BlockID back_block = *(cache_ptr - stride_slice);

          const BlockModel& currModel =
              game_context_.block_model_manager_->GetBlockModel(curr_block);
          const BlockModel& backModel =
              game_context_.block_model_manager_->GetBlockModel(back_block);

          bool blank_curr_model =
              !currModel.is_initialized_ || pos[axis] == kChunkDim;
          bool blank_back_model = !backModel.is_initialized_ || pos[axis] == 0;

          // Check if it is visible from the back and front
          if (!blank_curr_model) {
            for (size_t i = 0; i < currModel.elements_.size(); ++i) {
              face_visibility[i] = 0;
              const Cuboid& element = currModel.elements_[i];

              if (element.faces_[axis * 2 + 1].reference_texture_.length() == 0)
                continue;

              if (element.faces_[axis * 2 + 1].cull_face_ != -1) {
                if (!IsFaceVisible(element,
                                   element.faces_[axis * 2 + 1].cull_face_,
                                   cache_ptr))
                  continue;
              }
              face_visibility[i] = 1;
            }
          }
          // Check if it is visible from the back and front
          if (!blank_back_model) {
            for (size_t i = 0; i < backModel.elements_.size(); ++i) {
              face_visibility_back[i] = 0;
              const Cuboid& element = backModel.elements_[i];

              if (element.faces_[axis * 2].reference_texture_.length() == 0)
                continue;

              if (element.faces_[axis * 2].cull_face_ != -1) {
                if (!IsFaceVisible(element, element.faces_[axis * 2].cull_face_,
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

          for (int v = pos[axis_v] + 1; v < kChunkDim; ++v) {
            q_cache_ptr += stride_v;
            q_used_ptr += chunk_stride_v;
            // Check if they are the same
            BlockID currBlock2 = *q_cache_ptr;
            BlockID backBlock2 = *(q_cache_ptr - stride_slice);

            if (currBlock2 != curr_block || backBlock2 != back_block) break;

            *q_used_ptr = 0xFFU;
            v_length++;
          }

          // memset(used_block.data() + (pos[axis_u] << 4) + pos[axis_v],
          // vLength, vLength);

          q_cache_ptr = cache_ptr;
          q_used_ptr = used_ptr;
          for (int u = pos[axis_u] + 1; u < kChunkDim; ++u) {
            bool isValid = true;

            q_cache_ptr += stride_u;
            q_used_ptr += chunk_stride_u;

            const BlockID* q_cache_ptr_v = q_cache_ptr;

            for (int v = 0; v < v_length; ++v) {
              BlockID currBlock2 = *q_cache_ptr_v;
              BlockID backBlock2 = *(q_cache_ptr_v - stride_slice);

              q_cache_ptr_v += stride_v;

              if (currBlock2 != curr_block || backBlock2 != back_block) {
                isValid = false;
                break;
              }
            }

            if (!isValid) {
              break;
            }
            memset(q_used_ptr, v_length, v_length);
            ++u_length;
          }

          BlockPos q_pos = pos;

          // Memorize & Add Faces
          /*
           * Center wont have AO check as it is no sides
           * Do lighting checks when combining faces
           */
          if (!blank_curr_model) {
            for (size_t i = 0; i < currModel.elements_.size(); i++) {
              if (!face_visibility[i]) continue;
              const Cuboid& element = currModel.elements_[i];
              for (q_pos[axis_u] = pos[axis_u];
                   q_pos[axis_u] < pos[axis_u] + u_length; ++q_pos[axis_u]) {
                for (q_pos[axis_v] = pos[axis_v];
                     q_pos[axis_v] < pos[axis_v] + v_length; ++q_pos[axis_v]) {
                  AddFaceToMesh(element.faces_[axis * 2 + 1], axis * 2 + 1,
                                element.from_, element.to_,
                                currModel.ambient_occlusion_, q_pos);
                }
              }
            }
          }
          --q_pos[axis];
          if (!blank_back_model) {
            for (size_t i = 0; i < backModel.elements_.size(); i++) {
              if (!face_visibility_back[i]) continue;
              const Cuboid& element = backModel.elements_[i];
              for (q_pos[axis_u] = pos[axis_u];
                   q_pos[axis_u] < pos[axis_u] + u_length; ++q_pos[axis_u]) {
                for (q_pos[axis_v] = pos[axis_v];
                     q_pos[axis_v] < pos[axis_v] + v_length; ++q_pos[axis_v]) {
                  AddFaceToMesh(element.faces_[axis * 2], axis * 2,
                                element.from_, element.to_,
                                backModel.ambient_occlusion_, q_pos);
                }
              }
            }
          }

          q_pos = pos;

          pos[axis_v] += v_length - 1;
          v_ptr += v_length * stride_v;
          used_ptr += v_length;
        }
        u_ptr += stride_u;
      }
      slice_ptr += stride_slice;
    }
  }
}

void Mesh::ChunkMeshData::AddFaceToMesh(const BlockFace& face, uint8_t axis,
                                        glm::vec3 from, glm::vec3 to,
                                        bool allowAO, BlockPos pos) {
  // First get some of the general data
  int direction = axis & 1;
  int facing = axis >> 1;

  std::vector<BlockVertexFormat>& vertex_buffer =
      face.partially_transparent_pixel_ ? transparent_vertices_buffer_
                                        : vertices_buffer_;

  size_t& face_count = face.partially_transparent_pixel_
                           ? transparent_face_count_
                           : solid_face_count_;

  size_t vertex_index = face_count * 6;

  // Resize buffer if it is soo too small
  if (vertex_buffer.size() <= (face_count + 1) * 6) {
    vertex_buffer.resize(vertex_buffer.size() + kBufferStepSize);
  }

  // Now calculate the AO, lighting, colors
  int ao[4] = {0};  // 0: NN, 1: NP, 2: PN, 3: PP
  if (allowAO) {
    GetAO(axis, pos, ao[0], ao[1], ao[2], ao[3]);
  }

  float ao_multi[4] = {1.0f - static_cast<float>(3 - ao[0]) / 5.0f,
                       1.0f - static_cast<float>(3 - ao[1]) / 5.0f,
                       1.0f - static_cast<float>(3 - ao[2]) / 5.0f,
                       1.0f - static_cast<float>(3 - ao[3]) / 5.0f};

  int sky_light = chunk_->lighting_->GetLighting(pos);
  int block_light = 0;

  glm::vec3 tint_color{1.0f};
  if (face.tint_index_ != -1) {
    tint_color = glm::vec3(93.0f / 255.0f, 200.0f / 255.0f, 62.0f / 255.0f);
  }

  // Now compute the vertices
  auto CreateVertex = [&](const glm::ivec2& uv, float ao_multiplier) {
    BlockVertexFormat v;
    glm::vec4 color(tint_color * ao_multiplier, 1.0f);
    // Assuming BlockVertexFormat::Set now takes a vec4 for color
    v.SetColor(static_cast<int>(color.r * 255), static_cast<int>(color.g * 255),
               static_cast<int>(color.b * 255), 255);
    v.SetUV(face.texture_id_, uv.x, uv.y);
    v.SetLight(sky_light, block_light);
    return v;
  };

  glm::vec3 base_pos{pos.x, pos.y, pos.z};

  if (direction) {
    base_pos[facing] += from[facing];
  } else {
    base_pos[facing] += to[facing];
  }

  glm::ivec2 uv_coord_00 = face.uv_coord_00;
  glm::ivec2 uv_coord_01 = face.uv_coord_10;
  glm::ivec2 uv_coord_10 = face.uv_coord_01;
  glm::ivec2 uv_coord_11 = face.uv_coord_11;

  BlockVertexFormat v_00, v_10, v_11, v_01;
  v_00 = CreateVertex(uv_coord_00, ao_multi[0]);
  v_01 = CreateVertex(uv_coord_01, ao_multi[1]);
  v_10 = CreateVertex(uv_coord_10, ao_multi[2]);
  v_11 = CreateVertex(uv_coord_11, ao_multi[3]);

  // Now work on positions
  switch (facing) {
    case 0:  // x axis
      v_00.SetPos(base_pos.x, base_pos.y + from.y, base_pos.z + from.z);
      v_10.SetPos(base_pos.x, base_pos.y + to.y, base_pos.z + from.z);
      v_11.SetPos(base_pos.x, base_pos.y + to.y, base_pos.z + to.z);
      v_01.SetPos(base_pos.x, base_pos.y + from.y, base_pos.z + to.z);
      break;
    case 1:
      v_00.SetPos(base_pos.x + from.x, base_pos.y, base_pos.z + from.z);
      v_10.SetPos(base_pos.x + from.x, base_pos.y, base_pos.z + to.z);
      v_11.SetPos(base_pos.x + to.x, base_pos.y, base_pos.z + to.z);
      v_01.SetPos(base_pos.x + to.x, base_pos.y, base_pos.z + from.z);
      break;
    case 2:
      v_00.SetPos(base_pos.x + from.x, base_pos.y + from.y, base_pos.z);
      v_10.SetPos(base_pos.x + to.x, base_pos.y + from.y, base_pos.z);
      v_11.SetPos(base_pos.x + to.x, base_pos.y + to.y, base_pos.z);
      v_01.SetPos(base_pos.x + from.x, base_pos.y + to.y, base_pos.z);
  }

  if (ao[0] + ao[3] <= ao[1] + ao[2]) {
    // Flipped quad
    vertex_buffer[vertex_index + 0] = v_00;
    vertex_buffer[vertex_index + 1] = v_10;
    vertex_buffer[vertex_index + 2] = v_11;

    vertex_buffer[vertex_index + 3] = v_00;
    vertex_buffer[vertex_index + 4] = v_11;
    vertex_buffer[vertex_index + 5] = v_01;
  } else {
    // Normal quad
    vertex_buffer[vertex_index + 0] = v_00;
    vertex_buffer[vertex_index + 1] = v_10;
    vertex_buffer[vertex_index + 2] = v_01;

    vertex_buffer[vertex_index + 3] = v_01;
    vertex_buffer[vertex_index + 4] = v_10;
    vertex_buffer[vertex_index + 5] = v_11;
  }

  // Flip vertices for face culling
  if (direction == 0) {
    std::swap(vertex_buffer[vertex_index + 1], vertex_buffer[vertex_index + 2]);
    std::swap(vertex_buffer[vertex_index + 4], vertex_buffer[vertex_index + 5]);
  }
  ++face_count;
}

void Mesh::ChunkMeshData::GetAO(int direction, BlockPos pos, int& ao_00,
                                int& ao_01, int& ao_10, int& ao_11) {
  ao_00 = 3, ao_01 = 3, ao_10 = 3, ao_11 = 3;
  const int axis = direction >> 1;
  const int stride_u = kCacheStride[(axis + 1) % 3];
  const int stride_v = kCacheStride[(axis + 2) % 3];

  pos.IncrementSide(direction, 1);
  const int current_idx =
      kCacheDim2D * (pos.x + 1) + kCacheDim1D * (pos.y + 1) + (pos.z + 1);
  const BlockID* center_ptr = &chunk_cache_[current_idx];

  if (*(center_ptr + stride_u) != game_context_.blocks_->AIR) {
    ao_11--;
    ao_10--;
  }

  if (*(center_ptr - stride_u) != game_context_.blocks_->AIR) {
    ao_01--;
    ao_00--;
  }

  if (*(center_ptr + stride_v) != game_context_.blocks_->AIR) {
    ao_01--;
    ao_11--;
  }

  if (*(center_ptr - stride_v) != game_context_.blocks_->AIR) {
    ao_10--;
    ao_00--;
  }

  if (ao_01 == 1 ||
      *(center_ptr - stride_u + stride_v) != game_context_.blocks_->AIR) {
    ao_01--;
  }

  if (ao_00 == 1 ||
      *(center_ptr - stride_u - stride_v) != game_context_.blocks_->AIR) {
    ao_00--;
  }

  if (ao_11 == 1 ||
      *(center_ptr + stride_u + stride_v) != game_context_.blocks_->AIR) {
    ao_11--;
  }

  if (ao_10 == 1 ||
      *(center_ptr + stride_u - stride_v) != game_context_.blocks_->AIR) {
    ao_10--;
  }
}

// Checks if a block side is visible to the player
bool Mesh::ChunkMeshData::IsFaceVisible(const Cuboid& cube, int side,
                                        const BlockID* cache) {
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
        element.faces_[opposite_side].partially_transparent_pixel_ ||
        element.faces_[opposite_side].fully_transparent_pixel_ ||
        element.faces_[opposite_side].reference_texture_.empty()) {
      continue;
    }

    if (side & 1) {  // if the  block arent touching
      if (element.to_[axis] < 1.0) {
        continue;
      } else if (element.from_[axis] > 0) {
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
