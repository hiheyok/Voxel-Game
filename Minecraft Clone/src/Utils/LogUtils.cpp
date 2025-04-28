#include "Utils/LogUtils.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "FileManager/Files.h"
#include "Utils/Clock.h"
void LogUtils::MainLogger() {
    const uint64_t printLimit = 8192;

    std::string printOutput = "";

    while (!stop_) {
        if (!logs_cache_.empty()) {
            LogData log = logs_cache_.front();
            logs_cache_.pop_front();

            time_t timept = std::chrono::system_clock::to_time_t(log.time_);
            char* context = nullptr;

            std::string timestamp =
                std::string(strtok_s(ctime(&timept), "\n", &context));

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

            str = FormatMessage(messageSeverity, log.r_time_, timestamp,
                                log.subtype_, log.message_);
            printOutput += str + "\n";

            if (printOutput.size() > printLimit) {
                printf("%s", printOutput.c_str());
                file_ << printOutput;
                printOutput.clear();
            }
        } else {
            if (printOutput.size() != 0) {
                printf("%s", printOutput.c_str());
                file_ << printOutput;
                printOutput.clear();
            }

            std::unique_lock<std::mutex> lock{mutex_};
            cv_.wait_for(lock, std::chrono::milliseconds(100),
                         [this] { return !logs_cache_.empty() || stop_; });
            if (stop_) break;
            logs_cache_ = std::move(logs_);
            logs_.clear();
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

        logging_thread_ = std::thread(&LogUtils::MainLogger, this);
        LogInfo("Logger", "Started Logger");
    } else {
        LogError("Logger", "Logger Already Started");
    }
}

void LogUtils::LogError(std::string subtype, std::string message) {
    LogData log;
    log.type_ = LOG_TYPE_ERROR;
    log.message_ = message;
    log.time_ = std::chrono::system_clock::now();
    log.subtype_ = subtype;
    log.r_time_ =
        (std::chrono::high_resolution_clock::now() - init_time_).count();
    std::lock_guard<std::mutex> lock{mutex_};
    logs_.emplace_back(log);
    cv_.notify_one();
    throw std::runtime_error(subtype + " - " + message);
}

void LogUtils::LogWarn(std::string subtype, std::string message) {
    LogData log;
    log.type_ = LOG_TYPE_WARN;
    log.message_ = message;
    log.time_ = std::chrono::system_clock::now();
    log.subtype_ = subtype;
    log.r_time_ =
        (std::chrono::high_resolution_clock::now() - init_time_).count();
    std::lock_guard<std::mutex> lock{mutex_};
    logs_.emplace_back(log);
    cv_.notify_one();
}

void LogUtils::LogInfo(std::string subtype, std::string message) {
    LogData log;
    log.type_ = LOG_TYPE_INFO;
    log.message_ = message;
    log.time_ = std::chrono::system_clock::now();
    log.subtype_ = subtype;
    log.r_time_ =
        (std::chrono::high_resolution_clock::now() - init_time_).count();
    std::lock_guard<std::mutex> lock{mutex_};
    logs_.emplace_back(log);
    cv_.notify_one();
}

void LogUtils::LogDebug(std::string subtype, std::string message) {
    LogData log;
    log.type_ = LOG_TYPE_DEBUG;
    log.message_ = message;
    log.time_ = std::chrono::system_clock::now();
    log.subtype_ = subtype;
    log.r_time_ =
        (std::chrono::high_resolution_clock::now() - init_time_).count();
    std::lock_guard<std::mutex> lock{mutex_};
    logs_.emplace_back(log);
    cv_.notify_one();
}

void LogUtils::LogDebugf(std::string subtype, std::string message, ...) {
    va_list args;
    va_start(args, message);

    std::string formatedString = FormatString(message, args);

    va_end(args);

    LogData log;
    log.type_ = LOG_TYPE_DEBUG;
    log.message_ = formatedString;
    log.time_ = std::chrono::system_clock::now();
    log.subtype_ = subtype;
    log.r_time_ =
        (std::chrono::high_resolution_clock::now() - init_time_).count();
    std::lock_guard<std::mutex> lock{mutex_};
    logs_.emplace_back(log);
    cv_.notify_one();
}

void LogUtils::Stop() { stop_ = true; }

LogUtils::LogUtils() {
    buffer_ = new char[buffer_size_];
    Start();
}

LogUtils::~LogUtils() {
    logging_thread_.join();
    delete[] buffer_;
}

std::string LogUtils::FormatString(std::string in, ...) {
    va_list args;
    va_start(args, in);

    vsnprintf(buffer_, buffer_size_, in.c_str(), args);

    va_end(args);

    std::string out(buffer_);
    memset(buffer_, 0, out.size());
    return out;
}

std::string LogUtils::FormatMessage(std::string severity, long long time,
                                    std::string timestamp, std::string subtype,
                                    std::string message) {
    return FormatString("[ %lld NS ] [ %s ] [ %s / %s ]: %s", time,
                        timestamp.c_str(), severity.c_str(), subtype.c_str(),
                        message.c_str());
}