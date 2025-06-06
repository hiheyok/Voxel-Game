// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/Mesh/ChunkMesh.h"

#include <array>
#include <cstdint>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"
#include "Level/Block/Type/Fluid.h"
#include "Level/Chunk/Chunk.h"
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
  BlockPos local_pos{0, 0, 0};

  for (local_pos.x = 0; local_pos.x < kChunkDim; ++local_pos.x) {
    for (local_pos.z = 0; local_pos.z < kChunkDim; ++local_pos.z) {
      for (local_pos.y = 0; local_pos.y < kChunkDim; ++local_pos.y) {
        SetCachedBlockID(chunk_->GetBlockUnsafe(local_pos), local_pos);
      }
    }
  }

  local_pos = BlockPos{0, 0, 0};

  for (const auto& side : Directions<ChunkPos>()) {
    int axis = side.GetAxis();
    int direction = side.GetDirection() & 0b1;

    std::optional<ChunkContainer*> neighbor = chunk_->GetNeighbor(side);

    if (!neighbor.has_value()) {
      continue;
    }

    for (int u = 0; u < kChunkDim; u++) {
      for (int v = 0; v < kChunkDim; v++) {
        pos[axis] = direction * 15;
        pos[(axis + 1) % 3] = u;
        pos[(axis + 2) % 3] = v;

        local_pos[axis] = 17 - 17 * direction;
        local_pos[(axis + 1) % 3] = u + 1;
        local_pos[(axis + 2) % 3] = v + 1;

        BlockID block = neighbor.value()->GetBlockUnsafe(pos);
        SetCachedBlockID(block, local_pos - BlockPos{1, 1, 1});
      }
    }
  }
}

void Mesh::ChunkMeshData::SetChunk(Chunk* pChunk) { chunk_ = pChunk; }

void Mesh::ChunkMeshData::GenerateMesh() {
  // Initialize
  if (chunk_ == nullptr) {
    return;
  }
  Timer time;
  GenerateCache();
  cache_time_ = time.GetTimePassed_μs();
  GenerateFaceCollection();
  greedy_time_ = time.GetTimePassed_μs() - cache_time_;
}

// Loops through all the blocks in the chunk and check if each block side is
// visible. If a block side is visible, it generates the quad and puts it in the
// cache
void Mesh::ChunkMeshData::GenerateFaceCollection() {
  std::vector<uint8_t> faceVisibilityBack(4096, 0b00);
  std::vector<uint8_t> faceVisibility(4096, 0b00);
  std::array<uint8_t, kChunkSize2D> usedBlock{};

  for (int axis = 0; axis < 3; axis++) {
    int axisU = (axis + 2) % 3;
    int axisV = (axis + 1) % 3;
    BlockPos pos{0, 0, 0};

    for (pos[axis] = 0; pos[axis] <= kChunkDim; ++pos[axis]) {  // Slice
      usedBlock.fill(0);
      for (pos[axisU] = 0; pos[axisU] < kChunkDim; ++pos[axisU]) {
        for (pos[axisV] = 0; pos[axisV] < kChunkDim; ++pos[axisV]) {
          if (usedBlock[(pos[axisU] << 4) + pos[axisV]] != 0x00) {
            pos[axisV] += usedBlock[(pos[axisU] << 4) + pos[axisV]] - 1;
            continue;
          }

          usedBlock[(pos[axisU] << 4) + pos[axisV]] = 0xFF;

          BlockID currBlock = GetCachedBlockID(pos);
          --pos[axis];
          BlockID backBlock = GetCachedBlockID(pos);
          ++pos[axis];

          const BlockModel& currModel =
              game_context_.block_model_manager_->GetBlockModel(currBlock);
          const BlockModel& backModel =
              game_context_.block_model_manager_->GetBlockModel(backBlock);

          bool blankCurrModel =
              !currModel.is_initialized_ || pos[axis] == kChunkDim;
          bool blankBackModel = !backModel.is_initialized_ || pos[axis] == 0;

          // Check if it is visible from the back and front
          if (!blankCurrModel) {
            for (size_t i = 0; i < currModel.elements_.size(); ++i) {
              faceVisibility[i] = 0;
              const Cuboid& element = currModel.elements_[i];

              if (element.faces_[axis * 2 + 1].reference_texture_.length() == 0)
                continue;

              if (element.faces_[axis * 2 + 1].cull_face_ != -1) {
                if (!IsFaceVisible(element, pos,
                                   element.faces_[axis * 2 + 1].cull_face_))
                  continue;
              }

              faceVisibility[i] |= 0b1;
            }
          }
          // Check if it is visible from the back and front
          --pos[axis];
          if (!blankBackModel) {
            for (size_t i = 0; i < backModel.elements_.size(); ++i) {
              faceVisibilityBack[i] = 0;
              const Cuboid& element = backModel.elements_[i];

              if (element.faces_[axis * 2].reference_texture_.length() == 0)
                continue;

              if (element.faces_[axis * 2].cull_face_ != -1) {
                if (!IsFaceVisible(element, pos,
                                   element.faces_[axis * 2].cull_face_))
                  continue;
              }

              faceVisibilityBack[i] |= 0b1;
            }
          }
          ++pos[axis];

          // Spread

          int uLength = 1;
          int vLength = 1;

          BlockPos q_pos = pos;

          for (q_pos[axisV] = pos[axisV] + 1; q_pos[axisV] < kChunkDim;
               ++q_pos[axisV]) {
            // Check if they are the same
            const BlockID& currBlock2 = GetCachedBlockID(q_pos);
            --q_pos[axis];
            const BlockID& backBlock2 = GetCachedBlockID(q_pos);
            ++q_pos[axis];

            if (currBlock2 != currBlock || backBlock2 != backBlock) break;

            usedBlock[(q_pos[axisU] << 4) + q_pos[axisV]] = 0xFFU;
            vLength++;
          }

          // memset(usedBlock.data() + (pos[axisU] << 4) + pos[axisV],
          // vLength, vLength);

          q_pos = pos;

          for (q_pos[axisU] = pos[axisU] + 1; q_pos[axisU] < kChunkDim;
               ++q_pos[axisU]) {
            bool isValid = true;

            for (q_pos[axisV] = pos[axisV]; q_pos[axisV] < pos[axisV] + vLength;
                 ++q_pos[axisV]) {
              const BlockID& currBlock2 = GetCachedBlockID(q_pos);
              --q_pos[axis];
              const BlockID& backBlock2 = GetCachedBlockID(q_pos);
              ++q_pos[axis];

              if (currBlock2 != currBlock || backBlock2 != backBlock) {
                isValid = false;
                break;
              }
            }

            if (!isValid) {
              break;
            }

            memset(usedBlock.data() +
                       (static_cast<int64_t>(q_pos[axisU]) << 4) + pos[axisV],
                   vLength, vLength);

            ++uLength;
          }

          q_pos[0] = pos[0];
          q_pos[1] = pos[1];
          q_pos[2] = pos[2];

          // Memorize & Add Faces

          // TODO(hiheyok): Add some AO Checks and Greedy Meshing here

          /*
           * Center wont have AO check as it is no sides
           * Do lighting checks when combining faces
           */
          if (!blankCurrModel) {
            for (size_t i = 0; i < currModel.elements_.size(); i++) {
              if (faceVisibility[i] != 1) continue;
              const Cuboid& element = currModel.elements_[i];
              for (q_pos[axisU] = pos[axisU];
                   q_pos[axisU] < pos[axisU] + uLength; ++q_pos[axisU]) {
                for (q_pos[axisV] = pos[axisV];
                     q_pos[axisV] < pos[axisV] + vLength; ++q_pos[axisV]) {
                  AddFaceToMesh(element.faces_[axis * 2 + 1], axis * 2 + 1,
                                element.from_, element.to_,
                                currModel.ambient_occlusion_, q_pos);
                }
              }
            }
          }
          --q_pos[axis];
          if (!blankBackModel) {
            for (size_t i = 0; i < backModel.elements_.size(); i++) {
              if (faceVisibilityBack[i] != 1) continue;
              const Cuboid& element = backModel.elements_[i];
              for (q_pos[axisU] = pos[axisU];
                   q_pos[axisU] < pos[axisU] + uLength; ++q_pos[axisU]) {
                for (q_pos[axisV] = pos[axisV];
                     q_pos[axisV] < pos[axisV] + vLength; ++q_pos[axisV]) {
                  AddFaceToMesh(element.faces_[axis * 2], axis * 2,
                                element.from_, element.to_,
                                backModel.ambient_occlusion_, q_pos);
                }
              }
            }
          }

          q_pos = pos;

          if (vLength == kChunkDim &&
              uLength == kChunkDim) {  // Skip entire layer
            pos[axisV] = kChunkDim - 1;
            pos[axisU] = kChunkDim - 1;
          } else {
            pos[axisV] += vLength - 1;
          }
        }
      }
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

  float ao_multi[4] = {1.0f - static_cast<float>(ao[0]) / 5.0f,
                       1.0f - static_cast<float>(ao[1]) / 5.0f,
                       1.0f - static_cast<float>(ao[2]) / 5.0f,
                       1.0f - static_cast<float>(ao[3]) / 5.0f};

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

  glm::vec3 min_offset = from / 16.0f;
  glm::vec3 max_offset = to / 16.0f;
  glm::vec3 base_pos{pos.x, pos.y, pos.z};

  if (direction == 0) {
    base_pos[facing] += max_offset[facing];
  } else {
    base_pos[facing] += min_offset[facing];
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
      v_00.SetPos(base_pos.x, base_pos.y + min_offset.y,
                  base_pos.z + min_offset.z);
      v_10.SetPos(base_pos.x, base_pos.y + max_offset.y,
                  base_pos.z + min_offset.z);
      v_11.SetPos(base_pos.x, base_pos.y + max_offset.y,
                  base_pos.z + max_offset.z);
      v_01.SetPos(base_pos.x, base_pos.y + min_offset.y,
                  base_pos.z + max_offset.z);
      break;
    case 1:
      v_00.SetPos(base_pos.x + min_offset.x, base_pos.y,
                  base_pos.z + min_offset.z);
      v_10.SetPos(base_pos.x + min_offset.x, base_pos.y,
                  base_pos.z + max_offset.z);
      v_11.SetPos(base_pos.x + max_offset.x, base_pos.y,
                  base_pos.z + max_offset.z);
      v_01.SetPos(base_pos.x + max_offset.x, base_pos.y,
                  base_pos.z + min_offset.z);
      break;
    case 2:
      v_00.SetPos(base_pos.x + min_offset.x, base_pos.y + min_offset.y,
                  base_pos.z);
      v_10.SetPos(base_pos.x + max_offset.x, base_pos.y + min_offset.y,
                  base_pos.z);
      v_11.SetPos(base_pos.x + max_offset.x, base_pos.y + max_offset.y,
                  base_pos.z);
      v_01.SetPos(base_pos.x + min_offset.x, base_pos.y + max_offset.y,
                  base_pos.z);
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

const BlockID& Mesh::ChunkMeshData::GetCachedBlockID(
    BlockPos pos) const noexcept {
  return chunk_cache_[(pos.x + 1) * 18 * 18 + (pos.z + 1) * 18 + (pos.y + 1)];
}

void Mesh::ChunkMeshData::SetCachedBlockID(BlockID b, BlockPos pos) noexcept {
  chunk_cache_[(pos.x + 1) * (kChunkDim + 2) * (kChunkDim + 2) +
               (pos.z + 1) * (kChunkDim + 2) + (pos.y + 1)] = b;
}

void Mesh::ChunkMeshData::GetAO(int direction, BlockPos pos, int& ao_00,
                                int& ao_01, int& ao_10, int& ao_11) {
  ao_00 = 0;
  ao_01 = 0;
  ao_10 = 0;
  ao_11 = 0;

  int axis = direction >> 1;
  int facing = direction & 1;

  pos[axis] += 1 - 2 * facing;

  // Check up down left right

  int axis_1 = (axis + 1) % 3;
  int axis_2 = (axis + 2) % 3;

  // Check up
  pos[axis_1] += 1;

  if (GetCachedBlockID(pos) != game_context_.blocks_->AIR) {
    ao_11++;
    ao_10++;
  }

  pos[axis_1] -= 2;

  if (GetCachedBlockID(pos) != game_context_.blocks_->AIR) {
    ao_01++;
    ao_00++;
  }

  pos[axis_1] += 1;
  pos[axis_2] += 1;

  if (GetCachedBlockID(pos) != game_context_.blocks_->AIR) {
    ao_01++;
    ao_11++;
  }

  pos[axis_2] -= 2;

  if (GetCachedBlockID(pos) != game_context_.blocks_->AIR) {
    ao_10++;
    ao_00++;
  }

  pos[axis_2] += 1;

  // Check corners now
  pos[axis_1] += 1;
  pos[axis_2] += 1;

  if (GetCachedBlockID(pos) != game_context_.blocks_->AIR) ao_11++;

  pos[axis_1] -= 2;
  if (GetCachedBlockID(pos) != game_context_.blocks_->AIR) ao_01++;

  pos[axis_2] -= 2;

  if (GetCachedBlockID(pos) != game_context_.blocks_->AIR) ao_00++;

  pos[axis_1] += 2;

  if (GetCachedBlockID(pos) != game_context_.blocks_->AIR) ao_10++;
}

// Checks if a block side is visible to the player
bool Mesh::ChunkMeshData::IsFaceVisible(const Cuboid& cube, BlockPos block_pos,
                                        uint8_t side) {
  const uint8_t axis = (side >> 1);  // Get side
  const uint8_t axis1 = (axis + 1) % 3;
  const uint8_t axis2 = (axis + 2) % 3;
  const uint8_t oppositeSide = side ^ 1;  // Flip the leading bit
  BlockPos pos = block_pos;
  pos.IncrementSide(side, 1);

  const BlockModel& model =
      game_context_.block_model_manager_->GetBlockModel(GetCachedBlockID(pos));

  if (!model.is_initialized_) return true;

  for (size_t i = 0; i < model.elements_.size(); ++i) {
    const Cuboid& element = model.elements_[i];
    if (element.faces_[oppositeSide].cull_face_ !=
            oppositeSide ||  // TODO(hiheyok): Replace and with OR  later after
                             // figuring out fluid rendering
        element.faces_[oppositeSide].partially_transparent_pixel_ ||
        element.faces_[oppositeSide].fully_transparent_pixel_ ||
        element.faces_[oppositeSide].reference_texture_.empty()) {
      continue;
    }

    if (side & 1) {  // if the  block arent touching
      if (element.to_[axis] < 16) {
        continue;
      } else if (element.from_[axis] > 0) {
        continue;
      }
    }

    // Check if the faces aren't overlapping

    if (element.from_[axis1] <= cube.from_[axis1] &&
        element.to_[axis1] >= cube.to_[axis1] &&
        element.from_[axis2] <= cube.from_[axis2] &&
        element.to_[axis2] >= cube.to_[axis2]) {
      return false;
    }
  }
  return true;
}

bool Mesh::ChunkMeshData::CompareBlockSide(BlockPos pos, uint8_t side,
                                           BlockID b) {
  BlockPos new_pos = pos;
  new_pos.IncrementSide(side, 1);

  return GetCachedBlockID(new_pos) == b;
}
