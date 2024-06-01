#pragma once
#include "Chunk.h"
#include "Lighting/ChunkLighting.h"
#include "../../Utils/Containers/BitStorage.h"

class ChunkColumn { //Helps with stuff like lighting
private:
	std::vector<Chunk*> Column;
	std::vector<ChunkLightingContainer*> LightColumn;
	int ChunkColumnHeightMap[16 * 16]{};
public:

	ChunkColumn() {
		Column.resize(32);
		LightColumn.resize(32);
		memset(ChunkColumnHeightMap, 0, 16 * 16 * 4);
	}

	void AddChunk(Chunk* chunk, int RelativeHeightLevel) {
		Column[RelativeHeightLevel] = chunk;
		LightColumn[RelativeHeightLevel] = &chunk->Lighting;
		UpdateLighting(RelativeHeightLevel);
	}

	void UpdateChunk(int RelativeHeightLevel) {
		UpdateLighting(RelativeHeightLevel);
	}

	Chunk* GetChunk(int HeightLevel) {
		return Column[HeightLevel];
	}

	void UpdateLighting(int Height) {

		unsigned char MaxBrightness = ChunkLightingContainer::MaxLightLevel;

		int TargetChunkHeightmap[16 * 16]{}; //Heightmap for chunk; 17 == no blocks found

		for (int i = 0; i < 256; i++) {
			TargetChunkHeightmap[i] = 17;
		}

		Chunk* TargetChunk = Column[Height];
		int TargetChunkHeight = Height * 16;

		TargetChunk->Lighting.ResetLighting();

		if (TargetChunk == nullptr) return;
		
		//Get target chunk heightmap
		for (int x = 0; x < 16; x++) {
			for (int z = 0; z < 16; z++) {
				//For each area go
				for (int y = 15; y >= 0; y--) {
					BlockID b = TargetChunk->GetBlockUnsafe(x, y, z);
					if (!Blocks.getBlockType(b)->Properties->transparency) {
						TargetChunk->Lighting.EditLight(x, y, z, 6);
						TargetChunkHeightmap[x * 16 + z] = y;
						break;
					}
				}


			}
		}

		return;
		uint8_t DarkLightLevel = 8;

		//Update chunk column lighting

		for (int x = 0; x < 16; x++) {
			for (int z = 0; z < 16; z++) {// Iterate though all area
				//Check heightmap if there is any changes

				if (TargetChunkHeightmap[x * 16 + z] == 17) continue;

				//This assumes heightmap increases only

				int ColumnLightHeight = TargetChunkHeightmap[x * 16 + z] + TargetChunkHeight;

				if (ChunkColumnHeightMap[x * 16 + z] >= ColumnLightHeight) continue;

				ChunkColumnHeightMap[x * 16 + z] = ColumnLightHeight;

				//Work on target chunk first
				for (int y = TargetChunkHeightmap[x * 16 + z]; y >= 0; y--) {
					LightColumn[Height]->EditLight(x, y, z, DarkLightLevel);
				}


				////Work on rest
				//for (int cy = Height - 1; cy >= 0; cy--) {
				//	if (Column[cy] == nullptr) continue;

				//	int ChunkBlockHeight = cy * 16;
				//	
				//	for (int y = 16; y >= 0; y--) {
				//		LightColumn[Height]->EditLight(x, y, z, DarkLightLevel);
				//	}
				//}
			}
		}

		//Set chunks to update

	}

}; 