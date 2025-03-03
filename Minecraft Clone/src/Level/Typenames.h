#pragma once
#include <vector>
#include "../Utils/Containers/skaHashmap.h"

typedef unsigned int EventID;
typedef unsigned short int BlockID;
typedef unsigned long long int EntityUUID; //UUID
typedef unsigned short EntityTypeID;
typedef unsigned int ItemID;

 
template <class K, class V>
using FastHashMap = ska::flat_hash_map<K, V>;

template <class V>
using FastHashSet = ska::flat_hash_set<V>;

template <class T>
using FastVector = std::vector<T>;


constexpr int FRONT = 0x00;
constexpr int BACK = 0x01;
constexpr int TOP = 0x02;
constexpr int BOTTOM = 0x03;
constexpr int RIGHT = 0x04;
constexpr int LEFT = 0x05;

constexpr int  PX = 0x00;
constexpr int  NX = 0x01;
constexpr int  PY = 0x02;
constexpr int  NY = 0x03;
constexpr int  PZ = 0x04;
constexpr int  NZ = 0x05;

