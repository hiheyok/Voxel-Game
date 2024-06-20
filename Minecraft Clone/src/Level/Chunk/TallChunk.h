#pragma once
#include "Chunk.h"

class TallChunk {
private:
	std::vector<Chunk*> ChunkSubColumn;
public:
	TallChunk() {
		ChunkSubColumn.resize(16);
		for (int i = 0; i < 16; i++) ChunkSubColumn[i] = new Chunk;
	}

	void SetPosition(int x, int y, int z) {
		y = y * 16;
		for (int i = 0; i < 16; i++) {
			ChunkSubColumn[i]->SetPosition(x, y + i, z);
		}
	}

	int GetYPosition() {
		return ChunkSubColumn[0]->Position.y;
	}

	int GetXPosition() {
		return ChunkSubColumn[0]->Position.x;
	}

	int GetZPosition() {
		return ChunkSubColumn[0]->Position.z;
	}

	void SetBlockUnsafe(int x, int y, int z, BlockID block) {
		int ChunkIndex = y / 16;
		ChunkSubColumn[ChunkIndex]->SetBlockUnsafe(block, x, y & 0b1111, z);
	}

	BlockID GetBlockUnsafe(int x, int y, int z) {
		int ChunkIndex = y / 16;
		return ChunkSubColumn[ChunkIndex]->GetBlockUnsafe(x, y & 0b1111, z);
	}

	std::vector<Chunk*> getCubicChunks() {
		return ChunkSubColumn;
	}
};