#pragma once
#include <memory>
#include <vector>
#include <glm/vec2.hpp>

#include "Level/TerrainGeneration/Generators/Generator.h"

class FastNoiseLite;

class MountainGenerator : public WorldGenerator {
public:
    std::unique_ptr<FastNoiseLite> noise_;

    void Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) override;

    MountainGenerator();

    void GenerateEnvironment(const ChunkPos& pos, Chunk* chunk);

    void GenerateDecor(const ChunkPos& pos, Chunk* chunk);

    float GetNoise3D(glm::ivec3 ChunkCoordinate, glm::ivec3 RelativeBlockCoords, int samples, float frequency);

    float GetNoise2D(glm::ivec2 ChunkCoordinate, glm::ivec2 RelativeBlockCoords, int samples, float frequency);

    float ContinentialNoise(float n);

    float ErosionNoise(float n);

    float PeaksAndValley(float n);

    size_t GetIndex(std::vector<glm::vec2>& vec, float bottomBound);

    std::vector<glm::vec2> continentalness_interpolation_ {
        glm::vec2(0.f, 0.75f),
        glm::vec2(0.2f, 0.82f),
        glm::vec2(0.23f, 0.8f),
        glm::vec2(0.28f, 0.5f),
        glm::vec2(0.38f, 0.2f),
        glm::vec2(0.75f, 0.1f),
        glm::vec2(1.f, 0.3f),
    };

    std::vector<glm::vec2> erosionness_interpolation_ {
        glm::vec2(0.f, 0.75f),
        glm::vec2(0.2f, 0.82f),
        glm::vec2(0.23f, 0.8f),
        glm::vec2(0.28f, 0.5f),
        glm::vec2(0.38f, 0.2f),
        glm::vec2(0.75f, 0.1f),
        glm::vec2(1.f, 0.3f),
    };

    std::vector<glm::vec2> peaks_valley_interpolation_ {
        glm::vec2(0.f, 0.0f),
        glm::vec2(0.075f, 0.0f),
        glm::vec2(0.15f, 0.25f),
        glm::vec2(0.5f, 0.35f),
        glm::vec2(0.6f, 0.75f),
        glm::vec2(0.75f, 0.85f),
        glm::vec2(0.9f, 0.83f),
        glm::vec2(1.f, 0.84f),
    };
};