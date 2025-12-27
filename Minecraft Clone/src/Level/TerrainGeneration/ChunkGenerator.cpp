// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/TerrainGeneration/ChunkGenerator.h"

#include <functional>

#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "Level/Chunk/Chunk.h"
#include "Level/TerrainGeneration/Generators/GeneratorType.h"
#include "Level/TerrainGeneration/Generators/Generators.h"

ChunkGenerator::ChunkGenerator(GameContext& context, int64_t world_seed,
                               WorldGeneratorID gen_type)
    : context_{context} {
  world_generator_type_ = gen_type;

  gen_pool_ = std::make_unique<ThreadPool<ChunkPos, WorkerReturnType>>(
      context_.options_->world_gen_threads_, "World Generator",
      std::bind_front(&ChunkGenerator::Worker, this), 100);

  WorldGenerator::SetSeed(world_seed);
}

ChunkGenerator::~ChunkGenerator() = default;

std::vector<std::unique_ptr<Chunk>> ChunkGenerator::GetOutput() {
  std::vector<std::unique_ptr<Chunk>> output;
  for (auto& chunks : gen_pool_->GetOutput()) {
    output.insert(output.end(), std::make_move_iterator(chunks.begin()),
                  std::make_move_iterator(chunks.end()));
  }

  return output;
}

/*
 * Static function for the thread pool to use
 */
std::vector<std::unique_ptr<Chunk>> ChunkGenerator::Worker(ChunkPos task,
                                                           int workerId) {
  // Generate
  ChunkPos pos = task;
  std::vector<std::unique_ptr<Chunk>> output;

  if (!context_.generators_->GetGenerator(world_generator_type_)
           ->use_tall_chunks_) {
    std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(context_);
    chunk->SetPosition(pos);
    context_.generators_->GetGenerator(world_generator_type_)
        ->Generate(pos, chunk);

    output.push_back(std::move(chunk));
  } else {
    std::unique_ptr<TallChunk> chunk = std::make_unique<TallChunk>(context_);
    if (pos.y == 0) {
      context_.generators_->GetGenerator(world_generator_type_)
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
