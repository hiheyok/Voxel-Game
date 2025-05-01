#include "Level/TerrainGeneration/ChunkGenerator.h"

#include <functional>

#include "Level/Chunk/Chunk.h"
#include "Level/TerrainGeneration/Generators/GeneratorType.h"

std::vector<std::unique_ptr<Chunk>> ChunkGenerator::GetOutput() {
  std::vector<std::unique_ptr<Chunk>> output;
  for (auto& chunks : gen_pool_->GetOutput()) {
    output.insert(output.end(), std::make_move_iterator(chunks.begin()),
                  std::make_move_iterator(chunks.end()));
  }

  return output;
}

void ChunkGenerator::Start(int threadCount, int64_t worldSeedIn,
                           WorldGeneratorID worldGeneratorType) {
  world_generator_type_ = worldGeneratorType;

  gen_pool_ = std::make_unique<
      ThreadPool<ChunkPos, std::vector<std::unique_ptr<Chunk>>>>(
      threadCount, "World Generator",
      std::bind_front(&ChunkGenerator::Worker, this), 100);

  WorldGenerator::SetSeed(worldSeedIn);
}

void ChunkGenerator::Stop() { gen_pool_->Stop(); }

/*
 * Static function for the thread pool to use
 */
std::vector<std::unique_ptr<Chunk>> ChunkGenerator::Worker(ChunkPos task) {
  // Generate
  ChunkPos pos = task;
  std::vector<std::unique_ptr<Chunk>> output;

  if (!g_generators.GetGenerator(world_generator_type_)->use_tall_chunks_) {
    std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();
    chunk->SetPosition(pos);
    g_generators.GetGenerator(world_generator_type_)->Generate(pos, chunk);

    output.push_back(std::move(chunk));
  } else {
    std::unique_ptr<TallChunk> chunk = std::make_unique<TallChunk>();
    if (pos.y == 0) {
      g_generators.GetGenerator(world_generator_type_)
          ->GenerateTall(pos, chunk);
    }
    output.insert(output.end(),
                  std::make_move_iterator(chunk->chunk_sub_column_.begin()),
                  std::make_move_iterator(chunk->chunk_sub_column_.end()));
  }

  return output;
}

void ChunkGenerator::Generate(const std::vector<ChunkPos>& pos) {
  gen_pool_->SubmitTask(pos);
}

void ChunkGenerator::Generate(ChunkPos pos) { gen_pool_->SubmitTask(pos); }
