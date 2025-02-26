#pragma once
#include "../Generator.h"

class DebugPositioning : public WorldGenerator {
public:
	DebugPositioning() {

	}

	Chunk* Generate(const ChunkPos& pos) override;
};