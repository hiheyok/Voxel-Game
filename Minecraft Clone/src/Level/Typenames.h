#pragma once
#include <vector>
#include "../Utils/Containers/skaHashmap.h"
#include "../Utils/Containers/robin_hood.h"

typedef unsigned int EventID;
typedef unsigned short int BlockID;
typedef unsigned long long int EntityUUID; //UUID
typedef unsigned short EntityTypeID;
typedef unsigned int ItemID;

 
template <class K, class V, class _Hash = robin_hood::hash<K>>
using FastHashMap = robin_hood::unordered_flat_map<K, V, _Hash>;

template <class V, class _Hash = robin_hood::hash<V>>
using FastHashSet = robin_hood::unordered_flat_set<V, _Hash>;

template <class T>
using FastVector = std::vector<T>;


inline constexpr int FRONT = 0x00;
inline constexpr int BACK = 0x01;
inline constexpr int TOP = 0x02;
inline constexpr int BOTTOM = 0x03;
inline constexpr int RIGHT = 0x04;
inline constexpr int LEFT = 0x05;

inline constexpr int  PX = 0x00;
inline constexpr int  NX = 0x01;
inline constexpr int  PY = 0x02;
inline constexpr int  NY = 0x03;
inline constexpr int  PZ = 0x04;
inline constexpr int  NZ = 0x05;

