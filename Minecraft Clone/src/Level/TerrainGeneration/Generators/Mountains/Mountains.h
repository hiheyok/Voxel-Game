#pragma once
#include "../Generator.h"
#include <glm/vec2.hpp>
#include <vector>

class MountainGenerator : public WorldGenerator {
public:
	FastNoiseLite noise_;

	Chunk* Generate(const ChunkPos& pos) override;

	MountainGenerator() {
		noise_.SetNoiseType(noise_.NoiseType_OpenSimplex2);
		noise_.SetFrequency(0.005f);
	}

	void GenerateEnvironment(const ChunkPos& pos, Chunk* chunk);

	void GenerateDecor(const ChunkPos& pos, Chunk* chunk);

	float GetNoise3D(glm::ivec3 ChunkCoordinate, glm::ivec3 RelativeBlockCoords, int samples, float frequency);

	float GetNoise2D(glm::ivec2 ChunkCoordinate, glm::ivec2 RelativeBlockCoords, int samples, float frequency);

	float ContinentialNoise(float n);

	float ErosionNoise(float n);

	float PeaksAndValley(float n);

	int GetIndex(std::vector<glm::vec2>& vec, float bottomBound);

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