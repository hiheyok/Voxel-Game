#pragma once

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdarg>
#include <deque>
#include <format>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>

#if defined(__GNUC__) || defined(__clang__)
#define FUNC_SIGNATURE __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define FUNC_SIGNATURE __FUNCSIG__
#else
#define FUNC_SIGNATURE __func__
#endif

enum class LogType { kDebug, kInfo, kError, kWarn };

#define LOG_ERROR(fmt, ...)                              \
  context_.logger_->Log(LogType::kError, FUNC_SIGNATURE, \
                        std::format(fmt, ##__VA_ARGS__))
#define LOG_WARN(fmt, ...)                              \
  context_.logger_->Log(LogType::kWarn, FUNC_SIGNATURE, \
                        std::format(fmt, ##__VA_ARGS__))
#define LOG_INFO(fmt, ...)                              \
  context_.logger_->Log(LogType::kInfo, FUNC_SIGNATURE, \
                        std::format(fmt, ##__VA_ARGS__))
#define LOG_DEBUG(fmt, ...)                              \
  context_.logger_->Log(LogType::kDebug, FUNC_SIGNATURE, \
                        std::format(fmt, ##__VA_ARGS__))

#define LOG_STATIC_ERROR(logger, fmt, ...) \
  logger->Log(LogType::kError, FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))
#define LOG_STATIC_WARN(logger, fmt, ...) \
  logger->Log(LogType::kWarn, FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))
#define LOG_STATIC_INFO(logger, fmt, ...) \
  logger->Log(LogType::kInfo, FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))
#define LOG_STATIC_DEBUG(logger, fmt, ...) \
  logger->Log(LogType::kDebug, FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))

struct LogData {
  LogType type_;
  std::string message_;
  std::string_view func_;  // function signatures are baked inside executable
  std::chrono::system_clock::time_point time_;
  int64_t r_time_ = 0;
};

class LogUtils {
 public:
  LogUtils();
  ~LogUtils();

  void Log(LogType type, std::string_view func, std::string msg);

 private:
  void MainLogger(std::stop_token stoken);

  static constexpr uint64_t kBufferSize = 4096;
  static constexpr uint64_t kPrintLimit = 8192;

  std::chrono::high_resolution_clock::time_point start_time_ =
      std::chrono::high_resolution_clock::now();

  std::jthread logging_thread_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::deque<LogData> logs_;
  std::deque<LogData> logs_cache_;
  std::ofstream file_;
  std::array<char, kBufferSize> buffer_;
};
