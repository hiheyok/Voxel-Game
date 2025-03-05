#pragma once

#include <thread>
#include <deque>
#include <string>
#include <chrono>
#include <mutex>
#include <fstream>
#include <cstdarg>

#define LOG_TYPE_DEBUG 0x00;
#define LOG_TYPE_INFO 0x01;
#define LOG_TYPE_ERROR 0x02;
#define LOG_TYPE_WARN 0x03;

struct LogData {
	int type_ = NULL;
	int thread = NULL;
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

	void LogError(std::string Subtype, std::string Message);
	void LogWarn(std::string Subtype, std::string Message);
	void LogInfo(std::string Subtype, std::string Message);
	void LogDebug(std::string Subtype, std::string Message);
	void LogDebugf(std::string Subtype, std::string Message,...);

	std::thread logging_thread_;
private:
	void MainLogger();

	std::string FormatString(std::string in, ...);
	std::string FormatMessage(std::string severity, long long time, std::string timestamp, std::string subtype, std::string message);

	std::chrono::high_resolution_clock::time_point init_time_ = std::chrono::high_resolution_clock::now();

	std::mutex mutex_;
	bool stop_ = false;
	bool started_ = false;
	std::deque<LogData> logs_;
	std::deque<LogData> logs_cache_;
	std::ofstream file_;
	char* buffer_;
	const uint64_t buffer_size_ = 4096;
};

extern LogUtils g_logger;
