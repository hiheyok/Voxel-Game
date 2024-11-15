#pragma once
#include "Chunk.h"
#include "Lighting/ChunkLighting.h"
#include "../../Utils/Containers/BitStorage.h"
#include "Heightmap/Heightmap.h"
#include <glm/vec2.hpp>
#include <intrin.h>




typedef uint64_t ChunkColumnID;

class ChunkColumn { //Helps with stuff like lighting
private:
	std::vector<Chunk*> Column;
	std::vector<ChunkLightingContainer*> LightColumn;
	Heightmap ColumnHeightmap;
	glm::ivec2 Position;
public:
	std::vector<bool> LightDirty;

	Heightmap& getHeightmap() {
		return ColumnHeightmap;
	}

	void replaceLightContainer(int y, ChunkLightingContainer* c) {
		if (LightColumn[y] == nullptr) {
			Logger.LogError("Chunk", "Unable to update lighting!");
			delete c;
			return;
		}

		ChunkLightingContainer* l = LightColumn[y];
		l->ReplaceData(c->getData());
		delete c;
	}

	ChunkColumn() {
		Column.resize(32, nullptr);
		LightColumn.resize(32, nullptr);
		LightDirty.resize(32, false);
		ColumnHeightmap.init();
	}

	static ChunkColumnID getColumnID(int x, int z) {
		
	}

	void AddChunk(Chunk* chunk, int RelativeHeightLevel) {
		Column[RelativeHeightLevel] = chunk;
		LightColumn[RelativeHeightLevel] = &chunk->Lighting;
		UpdateHeightmap(RelativeHeightLevel);
	}

	void UpdateChunk(int RelativeHeightLevel) {
		UpdateHeightmap(RelativeHeightLevel);
	}

	Chunk* GetChunk(int HeightLevel) {
		return Column[HeightLevel];
	}

	inline int16_t findSurfaceHeight(uint8_t x, uint8_t z, uint8_t StartingChunk = 31) {
		for (int currChunk = StartingChunk;  currChunk >= 0; --currChunk) {
			Chunk* curr = Column[currChunk];
			
			if (curr == nullptr) continue;

			for (int y = 15; y >= 0; y--) {
				if (curr->GetBlockUnsafe(x, y, z) != Blocks.AIR) {
					return currChunk * 16 + y;
				}
			}
		}
		return -1;
	}

	inline int16_t findSurfaceHeightSingleChunk(uint8_t p_height, uint8_t x, uint8_t z) {
		Chunk* currChunk = Column[p_height];
		if (currChunk) return -1;
		for (int y = 15; y >= 0; y--) {
			if (currChunk->GetBlockUnsafe(x, y, z) != Blocks.AIR) return y;
		}
		return -1;
	}

	inline void UpdateHeightmapSingleBlock(int p_height,BlockID p_block, uint8_t x, uint8_t y, uint8_t z) {
		int currHeight = ColumnHeightmap.get(z, y);

		//tmp
		for (int i = 0; i <= p_height; i++) {
			if (Column[i] != nullptr) {
				LightDirty[i] = true;
			}
		}

		if (p_height * 16 + y < currHeight) {
			return;
		}
		
		if (p_block == Blocks.AIR) {
			if (currHeight == p_height) {
				uint16_t surfaceLevel = findSurfaceHeight(x, z, p_height);
				if (surfaceLevel == -1) {
					surfaceLevel = 0;
				}
				ColumnHeightmap.edit(x, z, surfaceLevel);
			}
		} else {
			ColumnHeightmap.edit(x, z, p_height * 16 + y);
		}
	}

	void UpdateHeightmap(uint16_t Height) {
		for (int x = 0; x < 16; x++) {
			for (int z = 0; z < 16; z++) {
				int16_t surfaceLevel = findSurfaceHeight(x, z, Height);
				surfaceLevel = (surfaceLevel != -1) * surfaceLevel;
				ColumnHeightmap.edit(x, z, surfaceLevel);
			}
		}

		for (int i = 0; i <= Height; i++) {
			if (Column[i] != nullptr) {
				LightDirty[i] = true;
			}
		} 
//		printf("Avg Clock: %f\n", (double)ClockCount / EventCount);
	}
}; 
