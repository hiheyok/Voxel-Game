#pragma once
#include "../Generator.h"
#include "../../Noisemaps/Noisemap.h"
#include "../../Noisemaps/TerrainParameters.h"

class MinecraftTerrain : public WorldGenerator {
public:
	MinecraftTerrain() {
		NoiseMaps.Initialize(SEED);
	}

	Chunk* Generate(glm::ivec3 Position) override {
		Chunk* chunk = new Chunk;
		chunk->Position = Position;

		IslandLayer(chunk);

		return chunk;
	}
private:
	NoiseMapTypes NoiseMaps;
	TerrainParamters Paramters;

	glm::vec3 VecFloor(glm::vec3 v) {
		return glm::vec3(floor(v.x), floor(v.y), floor(v.z));
	}

	void IslandLayer(Chunk* chunk) {
		glm::vec3 Position4096 = VecFloor(chunk->Position / 4);
		glm::vec3 Position2048 = VecFloor(chunk->Position / 2);
		glm::ivec3 BlockPosition = chunk->Position * 16;

		float NoiseDensity2D = 0;
		NoiseDensity2D += NoiseMaps.IslandLayer2048.GetNoise(Position2048.x, Position2048.y, Position2048.z);
		NoiseDensity2D += NoiseMaps.IslandLayer4096.GetNoise(Position4096.x, Position4096.y, Position4096.z);
		NoiseDensity2D = (NoiseDensity2D + 2.f) / 4.f;

		float HeightMap2048 = NoiseDensity2D * Paramters.HeightIntensity + Paramters.HeightOffset;
		
		for (int y = BlockPosition.y; y < BlockPosition.y + 16; y++) {
			if (y > HeightMap2048) {
				break;
			}

			for (int x = 0; x < 16; x++) {
				for (int z = 0; z < 16; z++) {
					chunk->SetBlockUnsafe(Blocks.STONE, x, y - BlockPosition.y, z);
				}
			}
		}
	}

};