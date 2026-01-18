// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <stdint.h>

#include <bit>
#include <functional>
#include <typeinfo>
#include <vector>

#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Registry/ResourceLocation.h"
#include "Utils/Containers/robin_hood.h"
#include "Utils/Iterators.h"
#include "Utils/LogUtils.h"

#if defined(_MSC_VER)
#include <immintrin.h>
#define restrict
#elif defined(__GNUC__) || defined(__clang__)
#include <x86intrin.h>
#define restrict __restrict__
#else
#define restrict
#endif

#if defined(_MSC_VER)
#define FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define FORCEINLINE __attribute__((always_inline)) inline
#else
#define FORCEINLINE inline
#endif

typedef uint32_t EventID;
typedef uint16_t BlockID;
typedef int64_t EntityUUID;  // UUID
typedef uint64_t RegionID;
typedef uint32_t ItemID;
typedef uint64_t WorldGeneratorID;
typedef int32_t EntityModelId;

// ECS

typedef int32_t EntityComponentId;

template <class K, class V, class _Hash = robin_hood::hash<K>>
using FastHashMap =
    robin_hood::unordered_flat_map<K, V, _Hash, std::equal_to<>>;

template <class V, class _Hash = robin_hood::hash<V>>
using FastHashSet = robin_hood::unordered_flat_set<V, _Hash, std::equal_to<>>;

constexpr int kChunkDim = 16;
constexpr int kChunkSize2D = kChunkDim * kChunkDim;
constexpr int kChunkSize3D = kChunkSize2D * kChunkDim;
constexpr int kChunkDimLog2 =
    std::bit_width(static_cast<uint32_t>(kChunkDim)) - 1;

constexpr int kRegionDim = 32;
constexpr int kRegionSize2D = kRegionDim * kRegionDim;
constexpr int kRegionSize3D = kRegionSize2D * kRegionDim;
constexpr int kRegionDimLog2 =
    std::bit_width(static_cast<uint32_t>(kRegionDim)) - 1;

constexpr int64_t kWorldSeed = -501575345763903LL;
// -501575345763903LL;//-501575345763903LL;//-1587754402LL

constexpr int8_t kMaxLightLevel = 15;
constexpr double kDegToRad = 0.017453292;

template <typename Target, typename Base>
bool instanceof(Base* obj) {
  return dynamic_cast<Target*>(obj) != nullptr;
}
