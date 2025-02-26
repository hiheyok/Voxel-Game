#include "DebugPositioning.h"


Chunk* DebugPositioning::Generate(const ChunkPos& pos) {
	Chunk* chunk = new Chunk;
	// Print location in binary

	ChunkPos newPos = pos;

	// Red -> x
	for (int i = 0; i < 16; ++i) {
		if (newPos.x & 1)
			chunk->SetBlockUnsafe(Blocks.RED_WOOL, i, 0, 0);
		if (newPos.y & 1)
			chunk->SetBlockUnsafe(Blocks.GREEN_WOOL, i, 1, 0);
		if (newPos.z & 1)
			chunk->SetBlockUnsafe(Blocks.BLUE_WOOL, i, 2, 0);

		newPos.x >>= 1;
		newPos.y >>= 1;
		newPos.z >>= 1;
	}

	chunk->SetBlockUnsafe(Blocks.RED_WOOL, 1, 4, 0);
	chunk->SetBlockUnsafe(Blocks.GREEN_WOOL, 0, 5, 0);
	chunk->SetBlockUnsafe(Blocks.BLUE_WOOL, 0, 4, 1);

	chunk->SetBlockUnsafe(Blocks.GOLD_BLOCK, 0, 3, 0);
	chunk->SetBlockUnsafe(Blocks.EMERALD_BLOCK, 15, 3, 0);

	for (int i = 1; i < 15; ++i) {
		if (i & 1) {
			chunk->SetBlockUnsafe(Blocks.BLACK_CONCRETE, i, 3, 0);
		}
		else {
			chunk->SetBlockUnsafe(Blocks.WHITE_CONCRETE, i, 3, 0);
		}
	}

	return chunk;
}