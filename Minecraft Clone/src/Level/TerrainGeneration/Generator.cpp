#include "Generator.h"
#include "Generators/GeneratorType.h"
#include "../Chunk/Chunk.h"

std::vector<std::unique_ptr<Chunk>> ChunkGeneration::GetOutput() {
    std::vector<std::unique_ptr<Chunk>> output;
    for (auto& chunks : gen_pool_->GetOutput()) {
        output.insert(output.end(), std::make_move_iterator(chunks.begin()), std::make_move_iterator(chunks.end()));
    }

    return output;
}

void ChunkGeneration::Start(int threadCount, long long int worldSeedIn) {
    gen_pool_ = std::make_unique<ThreadPool<std::pair<ChunkPos, WorldGeneratorID>,
        std::vector<std::unique_ptr<Chunk>>,
        ChunkGeneration::Worker>>(threadCount, "World Generator", 100);

    WorldGenerator::SetSeed(worldSeedIn);
}

void ChunkGeneration::Stop() {
    gen_pool_->Stop();
}

/*
* Static function for the thread pool to use
*/
std::vector<std::unique_ptr<Chunk>> ChunkGeneration::Worker(const std::pair<ChunkPos, WorldGeneratorID>& task ) {
    //Generate
    const ChunkPos& pos = task.first;
    const WorldGeneratorID& generatorUse = task.second;
    std::vector<std::unique_ptr<Chunk>> output;

    if (!g_generators.GetGenerator(generatorUse)->use_tall_chunks_) {
        std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();
        chunk->SetPosition(pos.x, pos.y, pos.z);
        g_generators.GetGenerator(generatorUse)->Generate(pos, chunk);

        output.push_back(std::move(chunk));
    }
    else {
        std::unique_ptr<TallChunk> chunk = std::make_unique<TallChunk>();
        if (pos.y == 0) {
            g_generators.GetGenerator(generatorUse)->GenerateTall(pos, chunk);
        }
        output.insert(output.end(), std::make_move_iterator(chunk->chunk_sub_column_.begin()), std::make_move_iterator(chunk->chunk_sub_column_.end()));
    }

    return output;
}


void ChunkGeneration::Generate(ChunkPos pos, WorldGeneratorID genType) {
    gen_pool_->SubmitTask(std::pair<ChunkPos, WorldGeneratorID>(pos, genType));
}

void ChunkGeneration::Generate(std::vector<ChunkPos> ids, WorldGeneratorID genTypeIn) {
    std::vector<std::pair<ChunkPos, WorldGeneratorID>> tasks;
    for (const auto& chunkId : ids) {
        tasks.emplace_back(chunkId, genTypeIn);
    }
    gen_pool_->SubmitTask(tasks);
}
