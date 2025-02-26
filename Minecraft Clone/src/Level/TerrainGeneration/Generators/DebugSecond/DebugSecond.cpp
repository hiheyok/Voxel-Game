#include "DebugSecond.h"


Chunk* DebugWorldSecond::Generate(const ChunkPos& pos) {
	Chunk* chunk = new Chunk;

	chunk->SetBlock(Blocks.COBBLESTONE, 8, 8, 8);

	ChunkPos scaledPos = pos;

	scaledPos.set(pos.x * 16, pos.y * 16, pos.z * 16);

	for (int gx = 0; gx < 16; gx++) {
		for (int gy = 0; gy < 16; gy++) {
			for (int gz = 0; gz < 16; gz++) {

				int index = gz * 256 + gy * 16 + gx;


				if (index % 5) {
					continue;
				}

				int x = gx + scaledPos.x;
				int y = gy + scaledPos.y;
				int z = gz + scaledPos.z;

				if ((abs(x) >= abs(z)) && (abs(x) >= abs(y))) {
					chunk->SetBlock(Blocks.BLUE_STAINED_GLASS, gx, gy, gz);
				}
				if ((abs(z) >= abs(x)) && (abs(z) >= abs(y))) {
					chunk->SetBlock(Blocks.DARK_OAK_PLANKS, gx, gy, gz);
				}
				if ((abs(y) >= abs(x)) && (abs(y) >= abs(z))) {
					chunk->SetBlock(Blocks.ORANGE_CONCRETE, gx, gy, gz);
				}
			}
		}
	}
	return chunk;
}