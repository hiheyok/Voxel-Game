#pragma once

#include <thread>
#include <deque>
#include <string>
#include <chrono>
#include <mutex>
#include <fstream>
#include <cstdarg>
#include <condition_variable>

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
    long long int r_time_ = 0;
};

class LogUtils {
public:

    LogUtils();
    ~LogUtils();

    void Start();
    void Stop();

    void LogError(std::string subtype, std::string message);
    void LogWarn(std::string subtype, std::string message);
    void LogInfo(std::string subtype, std::string message);
    void LogDebug(std::string subtype, std::string message);
    void LogDebugf(std::string subtype, std::string message,...);

private:
    void MainLogger();

    std::string FormatString(std::string in, ...);
    std::string FormatMessage(std::string severity, long long time, std::string timestamp, std::string subtype, std::string message);

    std::chrono::high_resolution_clock::time_point init_time_ = std::chrono::high_resolution_clock::now();

    std::thread logging_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
    bool started_ = false;
    std::deque<LogData> logs_;
    std::deque<LogData> logs_cache_;
    std::ofstream file_;
    char* buffer_;
    const uint64_t buffer_size_ = 4096;
};

extern LogUtils g_logger;
