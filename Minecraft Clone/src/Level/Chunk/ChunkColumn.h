#pragma once
#include "Chunk.h"
#include "Lighting/ChunkLighting.h"
#include "../../Utils/Containers/BitStorage.h"
#include "Heightmap/Heightmap.h"
#include <glm/vec2.hpp>
#include <intrin.h>

// TODO: Rework this refactor ChunkColumnPos system
class ChunkColumn { //Helps with stuff like lighting
private:
	std::vector<Chunk*> column_;
	std::vector<std::shared_ptr<ChunkLightingContainer>> light_column_;
	Heightmap column_heightmap_;
	// glm::ivec2 Position;
public:
	std::vector<bool> light_dirty_;

	Heightmap& GetHeightmap() {
		return column_heightmap_;
	}

	void ReplaceLightContainer(int y, std::shared_ptr<ChunkLightingContainer> c) {
		if (light_column_[y] == nullptr) {
			light_column_[y] = std::move(c);
			return;
		}

		light_column_[y]->ReplaceData(c->getData());
	}

	ChunkColumn() {
		column_.resize(32, nullptr);
		light_column_.resize(32, nullptr);
		light_dirty_.resize(32, false);
		column_heightmap_.Init();
	}

	void AddChunk(Chunk* chunk, int RelativeHeightLevel) {
		column_[RelativeHeightLevel] = chunk;
		light_column_[RelativeHeightLevel] = chunk->lighting_;
		UpdateHeightmap(RelativeHeightLevel);
	}

	void UpdateChunk(int RelativeHeightLevel) {
		UpdateHeightmap(RelativeHeightLevel);
	}

	Chunk* GetChunk(int HeightLevel) const {
		return column_[HeightLevel];
	}

	inline int16_t FindSurfaceHeight(uint8_t x, uint8_t z, uint8_t StartingChunk = 31) const {
		for (int currChunk = StartingChunk;  currChunk >= 0; --currChunk) {
			Chunk* curr = column_[currChunk];
			
			if (curr == nullptr) continue;

			for (int y = 15; y >= 0; y--) {
				if (curr->GetBlockUnsafe(x, y, z) != g_blocks.AIR) {
					return currChunk * 16 + y;
				}
			}
		}
		return -1;
	}

	/*
	Input is the y axis where the chunk is located at and the x and z block position relative to the chunk
	*/

	inline int16_t FindSurfaceHeightSingleChunk(uint8_t p_height, uint8_t x, uint8_t z) const {
		Chunk* currChunk = column_[p_height];
		if (currChunk == nullptr) return -1;
		for (int y = 15; y >= 0; y--) {
			if (currChunk->GetBlockUnsafe(x, y, z) != g_blocks.AIR) return y;
		}
		return -1;
	}

	inline void UpdateHeightmapSingleBlock(int p_height,BlockID p_block, uint8_t x, uint8_t y, uint8_t z) {
		int currHeight = column_heightmap_.Get(z, y);

		//tmp
		for (int i = 0; i <= p_height; i++) {
			if (column_[i] != nullptr) {
				light_dirty_[i] = true;
			}
		}

		if (p_height * 16 + y < currHeight) {
			return;
		}
		
		if (p_block == g_blocks.AIR) {
			if (currHeight == p_height) {
				int16_t surfaceLevel = FindSurfaceHeight(x, z, p_height);
				if (surfaceLevel == -1) {
					surfaceLevel = 0;
				}
				column_heightmap_.Edit(x, z, surfaceLevel);
			}
		} else {
			column_heightmap_.Edit(x, z, p_height * 16 + y);
		}
	}

	void UpdateHeightmap(uint16_t height) {
		for (int x = 0; x < 16; x++) {
			for (int z = 0; z < 16; z++) {
				int16_t surfaceLevel = FindSurfaceHeight(x, z, static_cast<uint8_t>(height));
				surfaceLevel = (surfaceLevel != -1) * surfaceLevel;
				column_heightmap_.Edit(x, z, surfaceLevel);
			}
		}

		for (int i = 0; i <= height; i++) {
			if (column_[i] != nullptr) {
				light_dirty_[i] = true;
			}
		} 
//		printf("Avg Clock: %f\n", (double)ClockCount / EventCount);
	}
}; 
