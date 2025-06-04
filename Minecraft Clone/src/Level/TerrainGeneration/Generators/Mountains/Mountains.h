// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <array>
#include <glm/vec2.hpp>
#include <memory>
#include <vector>

#include "Level/TerrainGeneration/Generators/Generator.h"

class FastNoiseLite;

class MountainGenerator : public WorldGenerator {
 public:
  explicit MountainGenerator(GameContext&);
  ~MountainGenerator();

  void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) override;
  void GenerateEnvironment(ChunkPos pos, Chunk* chunk);
  void GenerateDecor(ChunkPos pos, Chunk* chunk);
  float GetNoise3D(glm::ivec3 ChunkCoordinate, glm::ivec3 RelativeBlockCoords,
                   int samples, float frequency);
  float GetNoise2D(glm::ivec2 ChunkCoordinate, glm::ivec2 RelativeBlockCoords,
                   int samples, float frequency);

  float ContinentialNoise(float n);
  float ErosionNoise(float n);
  float PeaksAndValley(float n);

  template <size_t T>
  static constexpr size_t GetIndex(const std::array<glm::vec2, T>& arr,
                                   float bottomBound) {
    for (size_t i = 0; i < T; i++) {
      if (arr[i].x >= bottomBound) {
        if (i == 0) {
          return 0;
        } else {
          return i - 1;
        }
      }
    }

    return T - 1;
  }
  static constexpr std::array<glm::vec2, 7> continentalness_interpolation_{
      glm::vec2(0.f, 0.75f),  glm::vec2(0.2f, 0.82f), glm::vec2(0.23f, 0.8f),
      glm::vec2(0.28f, 0.5f), glm::vec2(0.38f, 0.2f), glm::vec2(0.75f, 0.1f),
      glm::vec2(1.f, 0.3f),
  };

  static constexpr std::array<glm::vec2, 7> erosionness_interpolation_{
      glm::vec2(0.f, 0.75f),  glm::vec2(0.2f, 0.82f), glm::vec2(0.23f, 0.8f),
      glm::vec2(0.28f, 0.5f), glm::vec2(0.38f, 0.2f), glm::vec2(0.75f, 0.1f),
      glm::vec2(1.f, 0.3f),
  };

  static constexpr std::array<glm::vec2, 8> peaks_valley_interpolation_{
      glm::vec2(0.f, 0.0f),   glm::vec2(0.075f, 0.0f), glm::vec2(0.15f, 0.25f),
      glm::vec2(0.5f, 0.35f), glm::vec2(0.6f, 0.75f),  glm::vec2(0.75f, 0.85f),
      glm::vec2(0.9f, 0.83f), glm::vec2(1.f, 0.84f),
  };

  std::unique_ptr<FastNoiseLite> noise_;
};
