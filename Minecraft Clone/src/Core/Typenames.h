#pragma once
#include <vector>
#include <typeinfo>
#include <functional>
#include "Utils/Containers/robin_hood.h"
#include "Utils/LogUtils.h"
#include "Core/Position/ChunkPos.h"
#include "Core/Position/Direction.h"
#include "Core/Registry/ResourceLocation.h"

typedef unsigned int EventID;
typedef unsigned short int BlockID;
typedef unsigned long long int EntityUUID; //UUID
typedef unsigned long long int RegionID;
typedef unsigned short EntityTypeID;
typedef unsigned int ItemID;
typedef uint64_t WorldGeneratorID;
 
template <class K, class V, class _Hash = robin_hood::hash<K>>
using FastHashMap = robin_hood::unordered_flat_map<K, V, _Hash>;

template <class V, class _Hash = robin_hood::hash<V>>
using FastHashSet = robin_hood::unordered_flat_set<V, _Hash>;

inline constexpr int FRONT = 0x00;
inline constexpr int BACK = 0x01;
inline constexpr int TOP = 0x02;
inline constexpr int BOTTOM = 0x03;
inline constexpr int RIGHT = 0x04;
inline constexpr int LEFT = 0x05;

inline constexpr int PX = 0x00;
inline constexpr int NX = 0x01;
inline constexpr int PY = 0x02;
inline constexpr int NY = 0x03;
inline constexpr int PZ = 0x04;
inline constexpr int NZ = 0x05;

inline constexpr int kChunkDim = 16;
inline constexpr int kChunkSize2D = kChunkDim * kChunkDim;
inline constexpr int kChunkSize3D = kChunkSize2D * kChunkDim;

inline constexpr int kRegionDim = 32;
inline constexpr int kRegionSize2D = kRegionDim * kRegionDim;
inline constexpr int kRegionSize3D = kRegionSize2D * kRegionDim;

inline constexpr long long kWorldSeed = 0;//;-501575345763903LL;//-501575345763903LL;//-1587754402LL

template <typename Target, typename Base>
bool instanceof(Base* obj) {
    return dynamic_cast<Target*>(obj) != nullptr;
}