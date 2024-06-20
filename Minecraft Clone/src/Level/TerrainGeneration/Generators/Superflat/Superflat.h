#pragma once
#include "../Generator.h"

class SuperflatWorld : public  WorldGenerator {
public:
	SuperflatWorld() {

	}

	Chunk* Generate(glm::ivec3 Position) override;
};