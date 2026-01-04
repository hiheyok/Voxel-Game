// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/TerrainGeneration/Generators/Mountains/Mountains.h"

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/TerrainGeneration/Generators/Generator.h"
#include "Utils/FastNoiseLite.h"
#include "Utils/Iterators.h"

using std::make_unique;
using std::unique_ptr;

MountainGenerator::MountainGenerator(GameContext& context)
    : WorldGenerator{context}, noise_{make_unique<FastNoiseLite>()} {
  noise_->SetNoiseType(noise_->NoiseType_OpenSimplex2);
  noise_->SetFrequency(0.005f);
}

MountainGenerator::~MountainGenerator() = default;

void MountainGenerator::Generate(ChunkPos pos, unique_ptr<Chunk>& chunk) {
  ChunkPos scaledPos = pos * kChunkDim;

  float heightBias = 50;
  float noiseOffset = 0.3;

  for (auto [x, z] : Product<2>(kChunkDim)) {
    float continental =
        ContinentialNoise(GetNoise2D({pos.x, pos.z}, {x, z}, 3, 0.3f));
    float erosion =
        ErosionNoise(GetNoise2D({pos.x, pos.z}, {x + 4345, z + 6443}, 3, 1.f)) /
        2.f;
    float pv = PeaksAndValley(
                   GetNoise2D({pos.x, pos.z}, {x + 65345, z + 12323}, 3, 4.f)) /
               8;

    for (int y = 0; y < kChunkDim; y++) {
      float gy = static_cast<float>(y + scaledPos.y);

      float n = GetNoise3D({pos.x, pos.y, pos.z}, {x, y, z}, 4, 1.f);

      n = n + noiseOffset;

      n += continental;
      n += erosion;
      n += (pv / (heightBias * (n + 0.5f))) * gy;

      n = n * exp(-gy / heightBias);

      if (n > 0.5f) {
        if (n < 0.54f) {
          chunk->SetBlockUnsafe(context_.blocks_->GRASS_BLOCK, {x, y, z});
          chunk->SetBlock(context_.blocks_->DIRT, {x, y - 1, z});
        } else {
          chunk->SetBlockUnsafe(context_.blocks_->STONE, {x, y, z});
        }
      }
    }
  }

  GenerateEnvironment(pos, chunk.get());
  GenerateDecor(pos, chunk.get());

  int gx = scaledPos.x;
  int gz = scaledPos.z;
  /// int gy = scaledPos.y;

  if (pos.y == 3) {
    int numBlocks = static_cast<int>(context_.blocks_->block_type_data_.size());

    for (auto [x, z] : Product<2>(kChunkDim)) {
      int px = x + gx;
      int pz = z + gz;

      if (((px & 0b1) == 1) || ((pz & 0b1) == 1)) {
        continue;
      }

      px = px / 2;
      pz = pz / 2;

      if ((px < 0) || (px >= 20)) {
        continue;
      }

      int b = px + pz * 20;

      if ((b < numBlocks) && (b >= 0)) {
        chunk->SetBlockUnsafe(b, {x, 3, z});
      }
    }
  }
}

void MountainGenerator::GenerateEnvironment(ChunkPos pos, Chunk* chunk) {
  for (auto [x, y, z] : Product<3>(kChunkDim)) {
    BlockPos block_pos{x, y, z};
    if (block_pos.y + pos.y * kChunkDim < 34) {
      if ((chunk->GetBlockUnsafe(block_pos) == context_.blocks_->AIR)) {
        chunk->SetBlockUnsafe(context_.blocks_->BLUE_CONCRETE, block_pos);
      }

      if ((chunk->GetBlockUnsafe(block_pos) == context_.blocks_->GRASS_BLOCK)) {
        chunk->SetBlockUnsafe(context_.blocks_->SAND, block_pos);
      }
    }
  }
}

void MountainGenerator::GenerateDecor(ChunkPos pos, Chunk* chunk) {
  const int tree_height = 3;

  for (auto [x, z] : Product<2>(kChunkDim)) {
    // Global Pos
    int gx = pos.x * kChunkDim + x;
    int gz = pos.z * kChunkDim + z;

    float TREE_MAP =
        (noise_->GetNoise(static_cast<float>(gx) * 100.f,
                          static_cast<float>(gz) * 100.f, 3453454.f) +
         1.f) /
        2.f;
    for (int y = 0; y < kChunkDim; y++) {
      if (chunk->GetBlock({x, y - 1, z}) == context_.blocks_->GRASS_BLOCK) {
        if (TREE_MAP <= 0.04) {
          for (auto [tx, tz] : Product<2>(-2, 3)) {
            if ((abs(tx) == 2) && (abs(tz) == 2)) continue;

            for (int ty = tree_height; ty <= tree_height + 1; ty++)
              chunk->SetBlock(context_.blocks_->OAK_LEAF,
                              {x + tx, y + ty, z + tz});
          }
          for (auto [tx, tz, ty] : Product<3>(
                   {{-1, 2}, {-1, 2}, {tree_height + 2, tree_height + 4}})) {
            if ((abs(tx) == 1) && (abs(tz) == 1) && (ty == tree_height + 3)) {
              continue;
            }
            chunk->SetBlock(context_.blocks_->OAK_LEAF,
                            {x + tx, y + ty, z + tz});
          }

          for (int ty = 0; ty < tree_height + 2; ty++) {
            chunk->SetBlock(context_.blocks_->OAK_LOG, {x, y + ty, z});
          }
        }
      }
    }
  }

  int radius = 40;
  int x0 = pos.x * kChunkDim;
  int x1 = (pos.x + 1) * kChunkDim;
  int y0 = pos.y * kChunkDim;
  int y1 = (pos.y + 1) * kChunkDim;
  int z0 = pos.z * kChunkDim;
  int z1 = (pos.z + 1) * kChunkDim;

  for (auto [x, y, z] : Product<3>({{x0, x1}, {y0, y1}, {z0, z1}})) {
    if ((x * x) + (y - 140) * (y - 140) + z * z <= radius * radius) {
      chunk->SetBlock(context_.blocks_->SAND,
                      {x - pos.x * kChunkDim, y - pos.y * kChunkDim,
                       z - pos.z * kChunkDim});
    }
  }

  // Create a roof to test lighting
  if (pos.y != 5) return;
  for (auto [x, z] : Product<2>({{x0, x1}, {z0, z1}})) {
    if ((x - 100) * (x - 100) + (z - 100) * (z - 100) <= 100 * 100) {
      chunk->SetBlock(context_.blocks_->WHITE_CONCRETE,
                      {x - pos.x * kChunkDim, 10, z - pos.z * kChunkDim});
    }

    // if (y == 90) {
    //     SetBlock(Blocks.SAND, x - cx, y - cy, z - cz);
    // }
  }
}

float MountainGenerator::GetNoise3D(glm::ivec3 ChunkCoordinate,
                                    glm::ivec3 RelativeBlockCoords, int samples,
                                    float frequency) {
  glm::vec3 GlobalBlockPosition = {ChunkCoordinate * kChunkDim +
                                   RelativeBlockCoords};

  GlobalBlockPosition *= frequency;

  float out = 0.0f;

  for (int i = 0; i < samples; i++) {
    float i_f = static_cast<float>(i);
    float n = noise_->GetNoise(GlobalBlockPosition.x * powf(2.0, i_f),
                               GlobalBlockPosition.y * powf(2.0, i_f),
                               GlobalBlockPosition.z * powf(2.0, i_f)) +
              1;

    n *= 0.5f;
    out += n * powf(0.5f, i_f);
  }

  out = out * ((-0.5f) / (powf(0.5, static_cast<float>(samples)) - 1));

  return out;
}
float MountainGenerator::GetNoise2D(glm::ivec2 ChunkCoordinate,
                                    glm::ivec2 RelativeBlockCoords, int samples,
                                    float frequency) {
  glm::vec2 GlobalBlockPosition = {ChunkCoordinate * kChunkDim +
                                   RelativeBlockCoords};

  GlobalBlockPosition *= frequency;

  float out = 0.0f;

  for (int i = 0; i < samples; i++) {
    float n = noise_->GetNoise(
                  GlobalBlockPosition.x * powf(2.0, static_cast<float>(i)),
                  GlobalBlockPosition.y * powf(2.0, static_cast<float>(i))) +
              1;
    n *= 0.5f;
    out += n * powf(0.5f, static_cast<float>(i));
  }

  out = out * ((-0.5f) / (powf(0.5f, static_cast<float>(samples)) - 1.0f));

  return out;
}

float MountainGenerator::ContinentialNoise(float n) {
  size_t index = GetIndex(continentalness_interpolation_, n);

  float x1 = continentalness_interpolation_[index].x;
  float y1 = continentalness_interpolation_[index].y;

  index++;

  float x2 = continentalness_interpolation_[index].x;
  float y2 = continentalness_interpolation_[index].y;

  float m = (y1 - y2) / (x1 - x2);

  float out = m * (n - x1) + y1;

  return out;
}

float MountainGenerator::ErosionNoise(float n) {
  size_t index = GetIndex(erosionness_interpolation_, n);

  float x1 = erosionness_interpolation_[index].x;
  float y1 = erosionness_interpolation_[index].y;

  index++;

  float x2 = erosionness_interpolation_[index].x;
  float y2 = erosionness_interpolation_[index].y;

  float m = (y1 - y2) / (x1 - x2);

  float out = m * (n - x1) + y1;

  return out;
}

float MountainGenerator::PeaksAndValley(float n) {
  size_t index = GetIndex(peaks_valley_interpolation_, n);

  float x1 = peaks_valley_interpolation_[index].x;
  float y1 = peaks_valley_interpolation_[index].y;

  index++;

  float x2 = peaks_valley_interpolation_[index].x;
  float y2 = peaks_valley_interpolation_[index].y;

  float m = (y1 - y2) / (x1 - x2);

  float out = m * (n - x1) + y1;

  return out;
}
