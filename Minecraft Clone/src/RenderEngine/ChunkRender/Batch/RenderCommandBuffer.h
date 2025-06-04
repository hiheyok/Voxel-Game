// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <algorithm>
#include <glm/vec3.hpp>
#include <vector>

#include "Core/Typenames.h"

struct DrawCommandIndirect {
  uint32_t count_;
  uint32_t instance_count_;
  uint32_t first_;
  uint32_t base_instance_;

  DrawCommandIndirect()
      : count_{0}, instance_count_{0}, first_{0}, base_instance_{0} {}

  DrawCommandIndirect(uint32_t count, uint32_t instanceCount, uint32_t first,
                      uint32_t baseInstance)
      : count_{count},
        instance_count_{instanceCount},
        first_{first},
        base_instance_{baseInstance} {}

  void set(uint32_t count, uint32_t instanceCount, uint32_t first,
           uint32_t baseInstance) {
    count_ = count;
    instance_count_ = instanceCount;
    first_ = first;
    base_instance_ = baseInstance;
  }
};