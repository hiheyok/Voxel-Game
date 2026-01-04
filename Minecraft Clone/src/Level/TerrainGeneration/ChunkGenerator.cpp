// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/TerrainGeneration/ChunkGenerator.h"

#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/TallChunk.h"
#include "Level/TerrainGeneration/Generators/GeneratorType.h"
#include "Utils/ThreadPool.h"

using std::make_unique;
using std::move;
using std::unique_ptr;
using std::vector;

ChunkGenerator::ChunkGenerator(GameContext& context, int64_t world_seed,
                               WorldGeneratorID gen_type)
    : context_{context} {
  world_generator_type_ = gen_type;

  gen_pool_ = make_unique<ThreadPool<ChunkPos, WorkerReturnType>>(
      context_.options_->world_gen_threads_, "World Generator",
      std::bind_front(&ChunkGenerator::Worker, this), 100);

  WorldGenerator::SetSeed(world_seed);
}

ChunkGenerator::~ChunkGenerator() = default;

vector<unique_ptr<Chunk>> ChunkGenerator::GetOutput() {
  vector<unique_ptr<Chunk>> output;
  for (auto& chunks : gen_pool_->GetOutput()) {
    output.insert(output.end(), std::make_move_iterator(chunks.begin()),
                  std::make_move_iterator(chunks.end()));
  }

  return output;
}

/*
 * Static function for the thread pool to use
 */
vector<unique_ptr<Chunk>> ChunkGenerator::Worker(ChunkPos task, int workerId) {
  // Generate
  ChunkPos pos = task;
  vector<unique_ptr<Chunk>> output;

  if (!context_.generators_->GetGenerator(world_generator_type_)
           ->use_tall_chunks_) {
    unique_ptr<Chunk> chunk = make_unique<Chunk>(context_);
    chunk->SetPosition(pos);
    context_.generators_->GetGenerator(world_generator_type_)
        ->Generate(pos, chunk);

    output.push_back(move(chunk));
  } else {
    unique_ptr<TallChunk> chunk = make_unique<TallChunk>(context_);
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

void ChunkGenerator::Generate(const vector<ChunkPos>& pos) {
  gen_pool_->SubmitTask(pos);
}

void ChunkGenerator::Generate(ChunkPos pos) { gen_pool_->SubmitTask(pos); }
