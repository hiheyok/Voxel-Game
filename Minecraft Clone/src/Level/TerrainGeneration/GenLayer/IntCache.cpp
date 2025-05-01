#include "Level/TerrainGeneration/GenLayer/IntCache.h"

thread_local int IntCache::intCacheSize = 256;
thread_local std::vector<std::unique_ptr<int[]>> IntCache::freeSmallArrays;
thread_local std::vector<std::unique_ptr<int[]>> IntCache::inUseSmallArrays;
thread_local std::vector<std::unique_ptr<int[]>> IntCache::freeLargeArrays;
thread_local std::vector<std::unique_ptr<int[]>> IntCache::inUseLargeArrays;
