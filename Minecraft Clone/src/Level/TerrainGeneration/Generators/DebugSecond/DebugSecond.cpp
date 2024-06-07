#include "DebugSecond.h"

using namespace glm;

Chunk* DebugWorldSecond::Generate(ivec3 Position) {
	Chunk* chunk = new Chunk;

	chunk->SetBlock(Blocks.COBBLESTONE, 8, 8, 8);

	Position *= 16;

	for (int gx = 0; gx < 16; gx++) {
		for (int gy = 0; gy < 16; gy++) {
			for (int gz = 0; gz < 16; gz++) {

				int index = gz * 256 + gy * 16 + gx;


				if (index % 5) {
					continue;
				}

				int x = gx + Position.x;
				int y = gy + Position.y;
				int z = gz + Position.z;

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
}