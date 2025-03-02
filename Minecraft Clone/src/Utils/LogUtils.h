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
	std::string message;
	std::string Subtype;
	std::chrono::system_clock::time_point time;
	long long int RTime = 0;
};

class LogUtils {
public:

	LogUtils() {
		m_buffer = new char[BUFFER_SIZE];
		Start();
	}

	~LogUtils() {
		Stop();
		delete[] m_buffer;
	}


	void Start();
	
	void Stop();

	void LogError(std::string Subtype, std::string Message);

	void LogWarn(std::string Subtype, std::string Message);

	void LogInfo(std::string Subtype, std::string Message);

	void LogDebug(std::string Subtype, std::string Message);
	void LogDebugf(std::string Subtype, std::string Message,...);

	std::thread LoggingThread;


private:

	std::mutex Mutex;

	bool stop = false;

	bool Started = false;

	void MainLogger();

	std::string formatString(std::string in,  ... ) {
		va_list args;
		va_start(args, in);

		vsnprintf(m_buffer, BUFFER_SIZE, in.c_str(), args);
	
		va_end(args);

		std::string out(m_buffer);
		memset(m_buffer, NULL, out.size());
		return out;
	}

	std::string formatMessage(std::string pSeverity, long long pTime, std::string pTimestamp, std::string pSubtype, std::string pMessage) {
		return formatString("[ %lld NS ] [ %s ] [ %s / %s ]: %s", pTime, pTimestamp.c_str(), pSeverity.c_str(), pSubtype.c_str(), pMessage.c_str());
	}

	std::chrono::high_resolution_clock::time_point InitTime = std::chrono::high_resolution_clock::now();
	
	std::deque<LogData> Logs;
	std::deque<LogData> LogsCache;

	std::ofstream file;

	char* m_buffer;
	const uint64_t BUFFER_SIZE = 4096;

};

extern LogUtils Logger;
