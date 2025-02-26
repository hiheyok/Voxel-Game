#pragma once
#include "../Generator.h"

class SuperflatWorld : public  WorldGenerator {
public:
	SuperflatWorld() {

	}

	Chunk* Generate(const ChunkPos& pos) override;
};