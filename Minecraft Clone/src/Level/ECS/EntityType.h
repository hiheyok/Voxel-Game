#pragma once

#include <cstdint>

enum class EntityType : uint8_t {
  kInvalidEntity,
  kFallingBlock,
  kPlayer,
  kHuman,
  kZombie,
  kMaxVal
};
