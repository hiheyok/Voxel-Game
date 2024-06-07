#pragma once
#include "../Generator.h"
#include <glm/vec2.hpp>
#include <vector>

struct MountainGenerator : WorldGenerator {
	MountainGenerator() {
		Noise.SetNoiseType(Noise.NoiseType_OpenSimplex2);
		Noise.SetFrequency(0.005f);
	}

	Chunk* Generate(glm::ivec3 Position) override;

	FastNoiseLite Noise;

	void GenerateEnvironment(glm::ivec3 Position, Chunk* chunk);

	void GenerateDecor(glm::ivec3 Position, Chunk* chunk);

	float getNoise3D(glm::ivec3 ChunkCoordinate, glm::ivec3 RelativeBlockCoords, int samples, float frequency);

	float getNoise2D(glm::ivec2 ChunkCoordinate, glm::ivec2 RelativeBlockCoords, int samples, float frequency);

	float continentialNoise(float n);

	float erosionNoise(float n);

	float peaksandvalley(float n);

	int getIndex(std::vector<glm::vec2>& vec, float bottomBound);

	std::vector<glm::vec2> ContinentalnessInterpolation{
		glm::vec2(0.f, 0.75f),
		glm::vec2(0.2f, 0.82f),
		glm::vec2(0.23f, 0.8f),
		glm::vec2(0.28f, 0.5f),
		glm::vec2(0.38f, 0.2f),
		glm::vec2(0.75f, 0.1f),
		glm::vec2(1.f, 0.3f),
	};

	std::vector<glm::vec2> ErosionnessInterpolation{
		glm::vec2(0.f, 0.75f),
		glm::vec2(0.2f, 0.82f),
		glm::vec2(0.23f, 0.8f),
		glm::vec2(0.28f, 0.5f),
		glm::vec2(0.38f, 0.2f),
		glm::vec2(0.75f, 0.1f),
		glm::vec2(1.f, 0.3f),
	};

	std::vector<glm::vec2> PeaksValleyInterpolation{
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