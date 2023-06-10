#include <iostream>
#include <iomanip>
#include "LogUtils.h"
#include "Clock.h"

using namespace std;

LogUtils Logger;

void LogUtils::MainLogger() {
	while (!stop) {
		if (!LogsCache.empty()) {

			LogData log = LogsCache.front();
			LogsCache.pop_front();

			time_t timept = std::chrono::system_clock::to_time_t(log.time);

			const char* timestamp = strtok(ctime(&timept), "\n");

			if (log.type == 0x01) {
				printf("[ %s NS ] [ %s ] [ INFO / %s ]: %s\n",to_string(log.RTime).c_str(), timestamp, log.Subtype.c_str(), log.message.c_str());
			}
			if (log.type == 0x03) {
				printf("[ %s NS ] [ %s ] [ WARN / %s ]: %s\n",to_string(log.RTime).c_str(),  timestamp, log.Subtype.c_str(), log.message.c_str());
			}
			if (log.type == 0x02) {
				printf("[ %s NS ] [ %s ] [ ERROR / %s ]: %s\n", to_string(log.RTime).c_str(), timestamp, log.Subtype.c_str(), log.message.c_str());
			}
			if (log.type == 0x00) {
				printf("[ %s NS ] [ %s ] [ DEBUG / %s ]: %s\n", to_string(log.RTime).c_str(), timestamp, log.Subtype.c_str(), log.message.c_str());
			}

			
		}
		else {
			if (!Logs.empty()) {
				Mutex.lock();
				LogsCache = Logs;
				Logs.clear();
				Mutex.unlock();
			}
			else {
				timerSleepNotPrecise(10);
			}
		}
	}
}

void LogUtils::Start() {
	if (!Started) {
		LoggingThread = std::thread(&LogUtils::MainLogger, this);
		LogInfo("Logger","Started Logger");
	}
	else {
		LogError("Logger", "Logger Already Started");
	}
	
}




void LogUtils::LogError(std::string Subtype, std::string Message) {
	LogData log;
	log.type = LOG_TYPE_ERROR;
	log.message = Message;
	log.time = chrono::system_clock::now();
	log.Subtype = Subtype;
	log.RTime = (std::chrono::high_resolution_clock::now() - InitTime).count();
	Mutex.lock();
	Logs.emplace_back(log);
	Mutex.unlock();
}

void LogUtils::LogWarn(std::string Subtype, std::string Message) {
	LogData log;
	log.type = LOG_TYPE_WARN;
	log.message = Message;
	log.time = chrono::system_clock::now();
	log.Subtype = Subtype;
	log.RTime = (std::chrono::high_resolution_clock::now() - InitTime).count();
	Mutex.lock();
	Logs.emplace_back(log);
	Mutex.unlock();
}

void LogUtils::LogInfo(std::string Subtype, std::string Message) {
	LogData log;
	log.type = LOG_TYPE_INFO;
	log.message = Message;
	log.time = chrono::system_clock::now();
	log.Subtype = Subtype;
	log.RTime = (std::chrono::high_resolution_clock::now() - InitTime).count();
	Mutex.lock();
	Logs.emplace_back(log);
	Mutex.unlock();
}

void LogUtils::LogDebug(std::string Subtype, std::string Message) {
	LogData log;
	log.type = LOG_TYPE_DEBUG;
	log.message = Message;
	log.time = chrono::system_clock::now();
	log.Subtype = Subtype;
	log.RTime = (std::chrono::high_resolution_clock::now() - InitTime).count();
	Mutex.lock();
	Logs.emplace_back(log);
	Mutex.unlock();
}

void LogUtils::Stop() {
	stop = true;
}

LogUtils* getLogger() {
	return &Logger;
}

