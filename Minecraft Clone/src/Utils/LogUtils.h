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

#define LOG_ERROR(fmt, ...) \
  context_.logger_->LogError(FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))
#define LOG_WARN(fmt, ...) \
  context_.logger_->LogWarn(FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))
#define LOG_INFO(fmt, ...) \
  context_.logger_->LogInfo(FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))
#define LOG_DEBUG(fmt, ...) \
  context_.logger_->LogDebug(FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))

#define LOG_STATIC_ERROR(logger, fmt, ...) \
  logger->LogError(FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))
#define LOG_STATIC_WARN(logger, fmt, ...) \
  logger->LogWarn(FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))
#define LOG_STATIC_INFO(logger, fmt, ...) \
  logger->LogInfo(FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))
#define LOG_STATIC_DEBUG(logger, fmt, ...) \
  logger->LogDebug(FUNC_SIGNATURE, std::format(fmt, ##__VA_ARGS__))

inline constexpr int LOG_TYPE_DEBUG = 0x00;
inline constexpr int LOG_TYPE_INFO = 0x01;
inline constexpr int LOG_TYPE_ERROR = 0x02;
inline constexpr int LOG_TYPE_WARN = 0x03;

struct LogData {
  int type_ = 0;
  int thread = 0;
  std::string message_;
  std::string subtype_;
  std::chrono::system_clock::time_point time_;
  int64_t r_time_ = 0;
};

class LogUtils {
 public:
  LogUtils();
  ~LogUtils();

  void LogError(std::string subtype, std::string message);
  void LogWarn(std::string subtype, std::string message);
  void LogInfo(std::string subtype, std::string message);
  void LogDebug(std::string subtype, std::string message);
  void LogDebugf(std::string subtype, std::string message, ...);

  template <typename... Args>
  void LogErrorf(std::string_view func, std::string_view fmt, Args&&... args);

  template <typename... Args>
  void LogWarnf(std::string_view func, std::string_view fmt, Args&&... args);

  template <typename... Args>
  void LogInfof(std::string_view func, std::string_view fmt, Args&&... args);

  template <typename... Args>
  void LogDebugf(std::string_view func, std::string_view fmt, Args&&... args);

 private:
  void MainLogger(std::stop_token stoken);

  std::string FormatString(std::string in, ...);
  std::string FormatMessage(std::string severity, int64_t time,
                            std::string timestamp, std::string subtype,
                            std::string message);

  static constexpr uint64_t kBufferSize = 4096;
  static constexpr uint64_t kPrintLimit = 8192;

  std::chrono::high_resolution_clock::time_point init_time_ =
      std::chrono::high_resolution_clock::now();

  std::jthread logging_thread_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::deque<LogData> logs_;
  std::deque<LogData> logs_cache_;
  std::ofstream file_;
  std::array<char, kBufferSize> buffer_;
};

template <typename... Args>
void LogUtils::LogInfof(std::string_view func, std::string_view fmt,
                        Args&&... args) {
  std::string msg =
      std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
  LogInfo(std::string(func), msg);
}

template <typename... Args>
void LogUtils::LogWarnf(std::string_view func, std::string_view fmt,
                        Args&&... args) {
  std::string msg =
      std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
  LogWarn(std::string(func), msg);
}

template <typename... Args>
void LogUtils::LogDebugf(std::string_view func, std::string_view fmt,
                         Args&&... args) {
  std::string msg =
      std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
  LogDebug(std::string(func), msg);
}

template <typename... Args>
void LogUtils::LogErrorf(std::string_view func, std::string_view fmt,
                         Args&&... args) {
  std::string msg =
      std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
  LogError(std::string(func), msg);
}
