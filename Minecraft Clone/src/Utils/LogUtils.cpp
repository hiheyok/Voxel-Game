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

	while (!stop_) {
		if (!logs_cache_.empty()) {

			LogData log = logs_cache_.front();
			logs_cache_.pop_front();

			time_t timept = std::chrono::system_clock::to_time_t(log.time_);

			string timestamp = string(strtok(ctime(&timept), "\n"));

			std::string str = "";
			std::string messageSeverity = "";

			if (log.type_ == 0x01) {
				messageSeverity = "INFO";
			} else if (log.type_ == 0x03) {
				messageSeverity = "WARNING";
			} else if (log.type_ == 0x02) {
				messageSeverity = "ERROR";
			} else if (log.type_ == 0x00) {
				messageSeverity = "DEBUG";
			}

			str = FormatMessage(messageSeverity, log.r_time_, timestamp, log.subtype_, log.message_);
			printOutput += str + "\n";

			if (printOutput.size() > printLimit) {
				printf("%s", printOutput.c_str());
				file_ << printOutput;
				printOutput.clear();
			}
		}
		else {
			if (printOutput.size() != 0) {
				printf("%s", printOutput.c_str());
				file_ << printOutput;
				printOutput.clear();
			}

			if (!logs_.empty()) {
				std::lock_guard<std::mutex> lock{ mutex_ };
				logs_cache_ = std::move(logs_);
				logs_.clear();
			}
			else {
				timerSleepNotPrecise(10);
			}
		}
	}
}

void LogUtils::Start() {
	if (!started_) {
		if (!FileManager::CheckFolder("Logs")) {
			FileManager::CreateFolder("Logs");
		}

		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
		auto str = "Logs//" + oss.str() + ".log";

		file_.open(str);

		LoggingThread = std::thread(&LogUtils::MainLogger, this);
		LogInfo("Logger","Started Logger");
	}
	else {
		LogError("Logger", "Logger Already Started");
	}
	
}

void LogUtils::LogError(std::string subtype, std::string message) {
	LogData log;
	log.type_ = LOG_TYPE_ERROR;
	log.message_ = message;
	log.time_ = chrono::system_clock::now();
	log.subtype_ = subtype;
	log.r_time_ = (std::chrono::high_resolution_clock::now() - init_time_).count();
	std::lock_guard<std::mutex> lock{ mutex_ };
	logs_.emplace_back(log);
}

void LogUtils::LogWarn(std::string subtype, std::string message) {
	LogData log;
	log.type_ = LOG_TYPE_WARN;
	log.message_ = message;
	log.time_ = chrono::system_clock::now();
	log.subtype_ = subtype;
	log.r_time_ = (std::chrono::high_resolution_clock::now() - init_time_).count();
	std::lock_guard<std::mutex> lock{ mutex_ };
	logs_.emplace_back(log);
}

void LogUtils::LogInfo(std::string subtype, std::string message) {
	LogData log;
	log.type_ = LOG_TYPE_INFO;
	log.message_ = message;
	log.time_ = chrono::system_clock::now();
	log.subtype_ = subtype;
	log.r_time_ = (std::chrono::high_resolution_clock::now() - init_time_).count();
	std::lock_guard<std::mutex> lock{mutex_};
	logs_.emplace_back(log);
}

void LogUtils::LogDebug(std::string subtype, std::string message) {
	LogData log;
	log.type_ = LOG_TYPE_DEBUG;
	log.message_ = message;
	log.time_ = chrono::system_clock::now();
	log.subtype_ = subtype;
	log.r_time_ = (std::chrono::high_resolution_clock::now() - init_time_).count();
	std::lock_guard<std::mutex> lock{ mutex_ };
	logs_.emplace_back(log);
}

void LogUtils::LogDebugf(std::string subtype, std::string message, ...) {
	va_list args;
	va_start(args, message);

	std::string formatedString = FormatString(message, args);

	va_end(args);

	LogData log;
	log.type_ = LOG_TYPE_DEBUG;
	log.message_ = formatedString;
	log.time_ = chrono::system_clock::now();
	log.subtype_ = subtype;
	log.r_time_ = (std::chrono::high_resolution_clock::now() - init_time_).count();
	std::lock_guard<std::mutex> lock{ mutex_ };
	logs_.emplace_back(log);
}

void LogUtils::Stop() {
	stop_ = true;
	
	
}

LogUtils* getLogger() {
	return &g_logger;
}

