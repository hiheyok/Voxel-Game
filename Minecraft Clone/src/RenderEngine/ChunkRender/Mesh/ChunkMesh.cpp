// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/Mesh/ChunkMesh.h"

#include <array>
#include <cstdint>
#include <utility>
#include <vector>

#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"
#include "Level/Block/Type/Fluid.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Light/LightStorage.h"
#include "RenderEngine/BlockModel/BlockModels.h"
#include "Utils/Timer/Timer.h"

static constexpr int kPositionOffset = 16;
static constexpr int kBitShiftAmount = 9;          // 27 bits
static constexpr int kNormBitOffset = 27;          // 2 bits
static constexpr int kTintBitOffset = 29;          // 1 bit
static constexpr int kTextureBitOffset = 10;       // 18 bits
static constexpr int kBlockShadingBitOffset = 28;  // 4 bits

Mesh::ChunkMeshData::ChunkMeshData() { chunk_cache_.fill(g_blocks.AIR); }

Mesh::ChunkMeshData::~ChunkMeshData() = default;

void Mesh::ChunkMeshData::Reset() {
  if (vertices_buffer_.size() != kBufferStepSize) {
    vertices_buffer_.resize(kBufferStepSize, 0);
  }

  if (transparent_vertices_buffer_.size() != kBufferStepSize) {
    transparent_vertices_buffer_.resize(kBufferStepSize, 0);
  }

  transparent_face_count_ = 0;
  solid_face_count_ = 0;

  chunk_cache_.fill(g_blocks.AIR);
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

          const BlockID& currBlock = GetCachedBlockID(pos);
          --pos[axis];
          const BlockID& backBlock = GetCachedBlockID(pos);
          ++pos[axis];

          const BlockModel& currModel = g_blocks.GetBlockModel(currBlock);
          const BlockModel& backModel = g_blocks.GetBlockModel(backBlock);

          bool blankCurrModel =
              !currModel.is_initialized_ || pos[axis] == kChunkDim;
          bool blankBackModel = !backModel.is_initialized_ || pos[axis] == 0;

          // Check if it is visible from the back and front
          if (!blankCurrModel) {
            for (int i = 0; i < currModel.elements_.size(); ++i) {
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
            for (int i = 0; i < backModel.elements_.size(); ++i) {
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
            for (int i = 0; i < currModel.elements_.size(); i++) {
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
            for (int i = 0; i < backModel.elements_.size(); i++) {
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

void Mesh::ChunkMeshData::AddFaceToMesh(const BlockFace& face, uint8_t axis_,
                                        glm::ivec3 from_, glm::ivec3 to_,
                                        bool allowAO, BlockPos pos) {
  uint8_t NN = 15, PN = 15, PP = 15, NP = 15;
  uint8_t direction = axis_ & 0b1;
  uint8_t facing = axis_ >> 1;

  int x = pos.x * kChunkDim;
  int y = pos.y * kChunkDim;
  int z = pos.z * kChunkDim;

  glm::ivec3 P0{x + from_[0] + kPositionOffset, y + from_[1] + kPositionOffset,
                z + from_[2] + kPositionOffset};
  glm::ivec3 P1{x + to_[0] + kPositionOffset, y + to_[1] + kPositionOffset,
                z + to_[2] + kPositionOffset};

  glm::ivec2 texNN = face.uv_coord_nn;
  glm::ivec2 texPN = face.uv_coord_np;
  glm::ivec2 texPP = face.uv_coord_pp;
  glm::ivec2 texNP = face.uv_coord_pn;

  texNN.y <<= 5;
  texNP.y <<= 5;
  texPP.y <<= 5;
  texPN.y <<= 5;

  P0.x <<= 0;
  P1.x <<= 0;
  P0.y <<= kBitShiftAmount;
  P1.y <<= kBitShiftAmount;
  P0.z <<= kBitShiftAmount * 2;
  P1.z <<= kBitShiftAmount * 2;

  glm::ivec3 tP0 = P0;
  glm::ivec3 tP1 = P1;

  P0.x = tP0[facing % 3];
  P1.x = tP1[facing % 3];
  P0.y = tP0[(facing + 1) % 3];
  P1.y = tP1[(facing + 1) % 3];
  P0.z = tP0[(facing + 2) % 3];
  P1.z = tP1[(facing + 2) % 3];

  if (direction == 0) std::swap(P0.x, P1.x);
  if (facing == 2) std::swap(texPN, texNP);

  uint32_t tex = face.texture_id_ << kTextureBitOffset;
  uint32_t norm = facing << kNormBitOffset;
  uint32_t tint = (static_cast<uint32_t>(!!face.tint_index_)) << kTintBitOffset;

  std::vector<uint32_t>& out = face.partially_transparent_pixel_
                                   ? transparent_vertices_buffer_
                                   : vertices_buffer_;

  uint64_t currIndex = face.partially_transparent_pixel_
                           ? transparent_face_count_++
                           : solid_face_count_++;

  if (out.size() <= (currIndex + 1) * 12)
    out.resize(out.size() + kBufferStepSize);

  // Get AO
  glm::u8vec4 AO = allowAO ? GetAO(axis_, pos) : glm::u8vec4{15, 15, 15, 15};

  PP = AO[0], PN = AO[1], NP = AO[2], NN = AO[3];

  out[currIndex * 12 + 0] = 0u | P0[0] | P0[1] | P0[2] | norm | tint;
  out[currIndex * 12 + 1] =
      0u | texNN.x | texNN.y | tex | (NN << kBlockShadingBitOffset);
  out[currIndex * 12 + 2] = 0u | P0[0] | P1[1] | P0[2] | norm | tint;
  out[currIndex * 12 + 3] =
      0u | texPN.x | texPN.y | tex | (PN << kBlockShadingBitOffset);
  out[currIndex * 12 + 4] = 0u | P0[0] | P0[1] | P1[2] | norm | tint;
  out[currIndex * 12 + 5] =
      0u | texNP.x | texNP.y | tex | (NP << kBlockShadingBitOffset);
  out[currIndex * 12 + 6] = 0u | P0[0] | P0[1] | P1[2] | norm | tint;
  out[currIndex * 12 + 7] =
      0u | texNP.x | texNP.y | tex | (NP << kBlockShadingBitOffset);
  out[currIndex * 12 + 8] = 0u | P0[0] | P1[1] | P0[2] | norm | tint;
  out[currIndex * 12 + 9] =
      0u | texPN.x | texPN.y | tex | (PN << kBlockShadingBitOffset);
  out[currIndex * 12 + 10] = 0u | P0[0] | P1[1] | P1[2] | norm | tint;
  out[currIndex * 12 + 11] =
      0u | texPP.x | texPP.y | tex | (PP << kBlockShadingBitOffset);

  if (direction == 0) {
    std::swap(out[currIndex * 12 + 2], out[currIndex * 12 + 4]);
    std::swap(out[currIndex * 12 + 3], out[currIndex * 12 + 5]);

    std::swap(out[currIndex * 12 + 8], out[currIndex * 12 + 10]);
    std::swap(out[currIndex * 12 + 9], out[currIndex * 12 + 11]);
  }
}

const BlockID& Mesh::ChunkMeshData::GetCachedBlockID(
    BlockPos pos) const noexcept {
  return chunk_cache_[(pos.x + 1) * 18 * 18 + (pos.z + 1) * 18 + (pos.y + 1)];
}

void Mesh::ChunkMeshData::SetCachedBlockID(BlockID b, BlockPos pos) noexcept {
  chunk_cache_[(pos.x + 1) * (kChunkDim + 2) * (kChunkDim + 2) +
               (pos.z + 1) * (kChunkDim + 2) + (pos.y + 1)] = b;
  // is_fluid_.set((x + 1) * (kChunkDim + 2) * (kChunkDim + 2) + (z + 1) *
  // (kChunkDim + 2) + (y + 1), g_blocks.GetBlockProperties(b).is_fluid_);
}

glm::u8vec4 Mesh::ChunkMeshData::GetAO(uint8_t direction, BlockPos block_pos) {
  static constexpr uint8_t kAmbientOcclusionStrength = 2;
  BlockPos pos = block_pos;

  char initial_lighting = chunk_->lighting_->GetLighting(pos);

  uint8_t PP{15}, PN{15}, NP{15}, NN{15};

  uint8_t axis = direction >> 1;
  uint8_t facing = direction & 1;

  pos[axis] += 1 - 2 * facing;

  // Check up down left right

  int axis1 = (axis + 1) % 3;
  int axis2 = (axis + 2) % 3;

  // Check up
  pos[axis1] += 1;

  if (GetCachedBlockID(pos) != g_blocks.AIR) {
    PP -= kAmbientOcclusionStrength;
    PN -= kAmbientOcclusionStrength;
  }

  pos[axis1] -= 2;

  if (GetCachedBlockID(pos) != g_blocks.AIR) {
    NP -= kAmbientOcclusionStrength;
    NN -= kAmbientOcclusionStrength;
  }

  pos[axis1] += 1;
  pos[axis2] += 1;

  if (GetCachedBlockID(pos) != g_blocks.AIR) {
    NP -= kAmbientOcclusionStrength;
    PP -= kAmbientOcclusionStrength;
  }

  pos[axis2] -= 2;

  if (GetCachedBlockID(pos) != g_blocks.AIR) {
    PN -= kAmbientOcclusionStrength;
    NN -= kAmbientOcclusionStrength;
  }

  pos[axis2] += 1;

  // Check corners now
  pos[axis1] += 1;
  pos[axis2] += 1;

  if (GetCachedBlockID(pos) != g_blocks.AIR) PP -= kAmbientOcclusionStrength;

  pos[axis1] -= 2;
  if (GetCachedBlockID(pos) != g_blocks.AIR) NP -= kAmbientOcclusionStrength;

  pos[axis2] -= 2;

  if (GetCachedBlockID(pos) != g_blocks.AIR) NN -= kAmbientOcclusionStrength;

  pos[axis1] += 2;

  if (GetCachedBlockID(pos) != g_blocks.AIR) PN -= kAmbientOcclusionStrength;

  if (PP >= (15 - initial_lighting))
    PP = PP - (15 - initial_lighting);
  else
    PP = 0;

  if (PN >= (15 - initial_lighting))
    PN = PN - (15 - initial_lighting);
  else
    PN = 0;

  if (NN >= (15 - initial_lighting))
    NN = NN - (15 - initial_lighting);
  else
    NN = 0;

  if (NP >= (15 - initial_lighting))
    NP = NP - (15 - initial_lighting);
  else
    NP = 0;

  return glm::u8vec4(PP, PN, NP, NN);
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

  const BlockModel& model = g_blocks.GetBlockModel(GetCachedBlockID(pos));

  if (!model.is_initialized_) return true;

  for (int i = 0; i < model.elements_.size(); ++i) {
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
