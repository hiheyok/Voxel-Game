#pragma once
#include "../Generator.h"

class SuperflatWorld : public  WorldGenerator {
public:
    SuperflatWorld() {

    }

    void Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) override;
};