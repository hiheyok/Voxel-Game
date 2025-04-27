#pragma once
#include <vector>
#include <typeinfo>
#include <functional>
#include <stdint.h>
#include <bit>
#include "Utils/Containers/robin_hood.h"
#include "Utils/LogUtils.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Position/Direction.h"
#include "Core/Registry/ResourceLocation.h"

typedef uint32_t EventID;
typedef uint16_t BlockID;
typedef uint64_t EntityUUID; //UUID
typedef uint64_t RegionID;
typedef uint16_t EntityTypeID;
typedef uint32_t ItemID;
typedef uint64_t WorldGeneratorID;
 
template <class K, class V, class _Hash = robin_hood::hash<K>>
using FastHashMap = robin_hood::unordered_flat_map<K, V, _Hash>;

template <class V, class _Hash = robin_hood::hash<V>>
using FastHashSet = robin_hood::unordered_flat_set<V, _Hash>;

constexpr int kChunkDim = 16;
constexpr int kChunkSize2D = kChunkDim * kChunkDim;
constexpr int kChunkSize3D = kChunkSize2D * kChunkDim;
constexpr int kChunkDimLog2 = std::bit_width((unsigned int)(kChunkDim)) - 1;

constexpr int kRegionDim = 32;
constexpr int kRegionSize2D = kRegionDim * kRegionDim;
constexpr int kRegionSize3D = kRegionSize2D * kRegionDim;

constexpr long long kWorldSeed = 0;//;-501575345763903LL;//-501575345763903LL;//-1587754402LL

template <typename Target, typename Base>
bool instanceof(Base* obj) {
    return dynamic_cast<Target*>(obj) != nullptr;
}