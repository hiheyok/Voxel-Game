#pragma once
#include <array>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class IntCache {
   public:
    static int* getIntCache(int size) {
        if (size <= 256) {
            if (freeSmallArrays.empty()) {
                inUseSmallArrays.push_back(std::make_unique<int[]>(256));
                return inUseSmallArrays.back().get();
            } else {
                std::unique_ptr<int[]> array =
                    std::move(freeSmallArrays.back());
                freeSmallArrays.pop_back();
                inUseSmallArrays.push_back(std::move(array));
                return inUseSmallArrays.back().get();
            }
        } else if (size > intCacheSize) {
            intCacheSize = size;  // add memory deallocators
            freeLargeArrays.clear();
            inUseLargeArrays.clear();
            inUseLargeArrays.push_back(std::make_unique<int[]>(intCacheSize));
            return inUseLargeArrays.back().get();
        } else if (freeLargeArrays.empty()) {
            inUseLargeArrays.push_back(std::make_unique<int[]>(intCacheSize));
            return inUseLargeArrays.back().get();
        } else {
            std::unique_ptr<int[]> array = std::move(freeLargeArrays.back());
            freeLargeArrays.pop_back();
            inUseLargeArrays.push_back(std::move(array));
            return inUseLargeArrays.back().get();
        }
    }

    static void resetIntCache() {
        if (!freeLargeArrays.empty()) {
            freeLargeArrays.pop_back();
        }

        if (!freeSmallArrays.empty()) {
            freeSmallArrays.pop_back();
        }

        for (auto& array : inUseLargeArrays) {
            freeLargeArrays.push_back(std::move(array));
        }
        for (auto& array : inUseSmallArrays) {
            freeSmallArrays.push_back(std::move(array));
        }

        inUseLargeArrays.clear();
        inUseSmallArrays.clear();
    }

    static std::string getCacheSizes() {
        return "cache: " + std::to_string(freeLargeArrays.size()) +
               ", tcache: " + std::to_string(freeSmallArrays.size()) +
               ", allocated: " + std::to_string(inUseLargeArrays.size()) +
               ", tallocated: " + std::to_string(inUseSmallArrays.size());
    }

   private:
    static thread_local int intCacheSize;
    static thread_local std::vector<std::unique_ptr<int[]>> freeSmallArrays;
    static thread_local std::vector<std::unique_ptr<int[]>> inUseSmallArrays;
    static thread_local std::vector<std::unique_ptr<int[]>> freeLargeArrays;
    static thread_local std::vector<std::unique_ptr<int[]>> inUseLargeArrays;
};