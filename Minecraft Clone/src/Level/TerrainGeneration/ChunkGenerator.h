// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <type_traits>

#include "Core/Typenames.h"
#include "Utils/FastNoiseLite.h"
#include "Utils/ThreadPool.h"

class Chunk;

class ChunkGenerator {
 public:
  ChunkGenerator(int thread_count, int64_t world_seed,
                 WorldGeneratorID gen_type);
  ~ChunkGenerator();

  ChunkGenerator(const ChunkGenerator&) = delete;
  ChunkGenerator(ChunkGenerator&&) = delete;
  ChunkGenerator& operator=(const ChunkGenerator&) = delete;
  ChunkGenerator& operator=(ChunkGenerator&&) = delete;

  void Generate(const std::vector<ChunkPos>& ids);
  void Generate(ChunkPos ids);

  std::vector<std::unique_ptr<Chunk>> GetOutput();

 private:
  /*
   * Worker submitting output as std::vector<Chunk*> to handle tall chunks
   * with multiple sub chunks for now
   */
  std::vector<std::unique_ptr<Chunk>> Worker(ChunkPos task);

  using WorkerReturnType =
      std::invoke_result_t<decltype(&ChunkGenerator::Worker), ChunkGenerator*,
                           ChunkPos>;

  std::unique_ptr<ThreadPool<ChunkPos, WorkerReturnType>> gen_pool_;

  WorldGeneratorID world_generator_type_;

  bool stop_ = false;
  bool collect_ready_ = false;
};
