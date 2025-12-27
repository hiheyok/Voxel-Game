// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>

// Mutex for thread-safe assertion output
namespace {
inline std::mutex& GetAssertMutex() {
  static std::mutex assert_mutex;
  return assert_mutex;
}
}  // namespace

// Custom assertion macro that builds message atomically to prevent garbling
// This prevents garbled output when assertions fail in multi-threaded contexts
#ifdef NDEBUG
  #define GAME_ASSERT(condition, message) ((void)0)
#else
  #define GAME_ASSERT(condition, message) \
    do { \
      if (!(condition)) { \
        std::lock_guard<std::mutex> lock(GetAssertMutex()); \
        char buffer[2048]; \
        std::snprintf(buffer, sizeof(buffer), \
          "\n========================================\n" \
          "[ASSERT FAILED]\n" \
          "Condition: %s\n" \
          "Message:   %s\n" \
          "File:      %s\n" \
          "Line:      %d\n" \
          "========================================\n\n", \
          #condition, message, __FILE__, __LINE__); \
        std::fputs(buffer, stderr); \
        std::fflush(stderr); \
        std::abort(); \
      } \
    } while(0)
#endif
