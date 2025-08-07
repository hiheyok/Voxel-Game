#pragma once

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdarg>
#include <deque>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>

#if defined(__GNUC__) || defined(__clang__)
#define FUNC_SIGNATURE __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define FUNC_SIGNATURE __FUNCSIG__
#else
#define FUNC_SIGNATURE __func__
#endif

#define LOG_ERROR(msg) context_.logger_->LogError(FUNC_SIGNATURE, (msg))
#define LOG_WARN(msg) context_.logger_->LogWarn(FUNC_SIGNATURE, (msg))
#define LOG_INFO(msg) context_.logger_->LogInfo(FUNC_SIGNATURE, (msg))
#define LOG_DEBUG(msg) context_.logger_->LogDebug(FUNC_SIGNATURE, (msg))

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
