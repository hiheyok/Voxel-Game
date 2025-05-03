// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include "Core/Typenames.h"

class PerformanceProfiler {
 public:
  PerformanceProfiler();

  void ProfileStart(uint64_t hash);

  void ProfileStart(std::string path);

  void ProfileStop(uint64_t hash);

  void ProfileStop(std::string path);

  // TOOD: Fix me
  void RegisterPaths(std::string path);

  uint64_t Hasher(std::string path);

  void LoadCache();

  void Print();

  void CondenseCache();

  void CombineCache(PerformanceProfiler profiler);

  std::vector<std::pair<uint64_t, uint64_t>> time_pass_cache_{};

  FastHashMap<uint64_t, std::string> string_hash_container_;
  FastHashMap<uint64_t, std::vector<std::string>>
      string_tokenized_hash_container_;

 private:
  std::stack<std::pair<uint64_t, uint64_t>> timer_stack_;

  struct PerformanceTree {
    PerformanceTree();
    explicit PerformanceTree(std::string name);

    void ChangeTime(std::vector<std::string>& path, int depth, double time);

    void print(int depth = 0) const;

    double time_passed_ = 0.0;
    std::string name_ = "";
    std::vector<std::unique_ptr<PerformanceTree>> nodes_{};
  };

  PerformanceTree root_{"root"};
  std::chrono::high_resolution_clock::time_point initial_time_;
};
