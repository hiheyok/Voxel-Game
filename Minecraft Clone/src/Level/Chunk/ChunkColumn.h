#pragma once
#include "Chunk.h"
#include "Lighting/ChunkLighting.h"
#include "../../Utils/Containers/BitStorage.h"
#include "Heightmap/Heightmap.h"
#include <glm/vec2.hpp>

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
		Column.resize(32);
		LightColumn.resize(32);
		LightDirty.resize(32);
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

	void UpdateHeightmap(int Height) {

		//if ((Column[Height]->Position.x == -1) && (Column[Height]->Position.z == -1) && (Height == 3)) {
		//	for (int x = 0; x < 16; x++) {
		//		for (int z = 0; z < 16; z++) {// Iterate though all area
		//			std::cout << x << ", " << z << ": " << ChunkColumnHeightMap[x * 16 + z] << "\n";
		//		}
		//	}
		//	std::cout << "UpdateHeight: " << Height << "\n";
		//}

		unsigned char MaxBrightness = ChunkLightingContainer::MaxLightLevel;

		int TargetChunkHeightmap[16 * 16]{}; //Heightmap for chunk; 17 == no blocks found

		for (int i = 0; i < 256; i++) {
			TargetChunkHeightmap[i] = 17;
		}

		Chunk* TargetChunk = Column[Height];
		int TargetChunkHeight = Height * 16;

		if (TargetChunk == nullptr) return;

		//Get target chunk heightmap
		for (int x = 0; x < 16; x++) {
			for (int z = 0; z < 16; z++) {
				//For each area go
				for (int y = 15; y >= 0; y--) {
					BlockID b = TargetChunk->GetBlockUnsafe(x, y, z);
					if (b != Blocks.AIR) {
						TargetChunkHeightmap[x * 16 + z] = y;
						break;
					}
				}


			}
		}
		

		for (int x = 0; x < 16; x++) {
			for (int z = 0; z < 16; z++) {// Iterate though all area
				//Check heightmap if there is any changes

				if (TargetChunkHeightmap[x * 16 + z] == 17) {
					continue;

				}


				int ColumnLightHeight = TargetChunkHeightmap[x * 16 + z] + TargetChunkHeight;

				if (ColumnHeightmap.get(x, z) < ColumnLightHeight) {
					ColumnHeightmap.edit(x, z, ColumnLightHeight);
				}
				else {
					int ChunkHeight = ColumnHeightmap.get(x, z) >> 4;

					if (ChunkHeight == Height) {
						ColumnHeightmap.edit(x, z, ColumnLightHeight);
					}
				}

				
			}
		}

		

		for (int i = 0; i <= Height; i++) {
			if (Column[i] != nullptr) {
				LightDirty[i] = true;
			}
		} 

		

	}

}; 