#include <iostream>
#include <iomanip>
#include "LogUtils.h"
#include "Clock.h"
#include <ctime>
#include "../FileManager/Files.h"
#include <sstream>
using namespace std;

void LogUtils::MainLogger() {

	const uint64_t printLimit = 8192;

	string printOutput = "";

	while (!stop) {
		if (!LogsCache.empty()) {

			LogData log = LogsCache.front();
			LogsCache.pop_front();

			time_t timept = std::chrono::system_clock::to_time_t(log.time);

			string timestamp = string(strtok(ctime(&timept), "\n"));

			std::string str = "";
			std::string messageSeverity = "";

			if (log.type == 0x01) {
				messageSeverity = "INFO";
			} else if (log.type == 0x03) {
				messageSeverity = "WARNING";
			} else if (log.type == 0x02) {
				messageSeverity = "ERROR";
			} else if (log.type == 0x00) {
				messageSeverity = "DEBUG";
			}

			str = formatMessage(messageSeverity, log.RTime, timestamp, log.Subtype, log.message);
			printOutput += str + "\n";

			if (printOutput.size() > printLimit) {
				printf("%s", printOutput.c_str());
				file << printOutput;
				printOutput.clear();
			}
		}
		else {
			if (printOutput.size() != 0) {
				printf("%s", printOutput.c_str());
				file << printOutput;
				printOutput.clear();
			}

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
		if (!FileManager::CheckFolder("Logs")) {
			FileManager::CreateFolder("Logs");
		}

		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
		auto str = "Logs//" + oss.str() + ".log";

		file.open(str);

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

