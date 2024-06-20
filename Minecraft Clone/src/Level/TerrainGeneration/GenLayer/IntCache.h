#pragma once
#include <vector>
#include <string>
#include <mutex>

class IntCache {
private:
    static int intCacheSize;
    static std::vector<int*> freeSmallArrays;
    static std::vector<int*> inUseSmallArrays;
    static std::vector<int*> freeLargeArrays;
    static std::vector<int*> inUseLargeArrays;

public:
    static int* getIntCache(int size) {
        std::lock_guard<std::mutex> lock(mutex);  // Add mutex lock for thread safety

        if (size <= 256) {
            if (freeSmallArrays.empty()) {
                int* array = new int[256];
                inUseSmallArrays.push_back(array);
                return array;
            }
            else {
                int* array = freeSmallArrays.back();
                freeSmallArrays.pop_back();
                inUseSmallArrays.push_back(array);
                return array;
            }
        }
        else if (size > intCacheSize) {
            intCacheSize = size; //add memory deallocators
            freeLargeArrays.clear();
            inUseLargeArrays.clear();
            int* array = new int[intCacheSize];
            inUseLargeArrays.push_back(array);
            return array;
        }
        else if (freeLargeArrays.empty()) {
            int* array = new int[intCacheSize];
            inUseLargeArrays.push_back(array);
            return array;
        }
        else {
            int* array = freeLargeArrays.back();
            freeLargeArrays.pop_back();
            inUseLargeArrays.push_back(array);
            return array;
        }
    }

    static void resetIntCache() {
        std::lock_guard<std::mutex> lock(mutex);  // Add mutex lock for thread safety

        if (!freeLargeArrays.empty()) {
            delete[] freeLargeArrays.back();
            freeLargeArrays.pop_back();
        }

        if (!freeSmallArrays.empty()) {
            delete[] freeSmallArrays.back();
            freeSmallArrays.pop_back();
        }

        for (auto array : inUseLargeArrays) {
            freeLargeArrays.push_back(array);
        }
        for (auto array : inUseSmallArrays) {
            freeSmallArrays.push_back(array);
        }

        inUseLargeArrays.clear();
        inUseSmallArrays.clear();
    }

    static std::string getCacheSizes() {
        std::lock_guard<std::mutex> lock(mutex);  // Add mutex lock for thread safety

        return "cache: " + std::to_string(freeLargeArrays.size()) +
            ", tcache: " + std::to_string(freeSmallArrays.size()) +
            ", allocated: " + std::to_string(inUseLargeArrays.size()) +
            ", tallocated: " + std::to_string(inUseSmallArrays.size());
    }

private:
    static std::mutex mutex; // Add mutex for thread safety
};

// Initialize static variables
__declspec(selectany) int IntCache::intCacheSize = 256;
__declspec(selectany) std::vector<int*> IntCache::freeSmallArrays;
__declspec(selectany) std::vector<int*> IntCache::inUseSmallArrays;
__declspec(selectany) std::vector<int*> IntCache::freeLargeArrays;
__declspec(selectany) std::vector<int*> IntCache::inUseLargeArrays;
__declspec(selectany) std::mutex IntCache::mutex;