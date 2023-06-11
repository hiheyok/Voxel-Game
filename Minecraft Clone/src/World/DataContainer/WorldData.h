#pragma once

#include "ChunkMapData.h"

class WorldData {
public:

	bool SetBlock(BlockID block, glm::ivec3 Position);

private:

	ChunkMap WorldChunkData;

};