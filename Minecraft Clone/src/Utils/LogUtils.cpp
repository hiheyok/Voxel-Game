#include "Utils/LogUtils.h"

#include <chrono>
#include <cstring>
#include <format>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "FileManager/Files.h"
#include "Utils/Clock.h"
LogUtils::LogUtils() : buffer_{'\0'} {
  if (!FileManager::CheckFolder("Logs")) {
    FileManager::CreateFolder("Logs");
  }

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
  auto str = "Logs//" + oss.str() + ".log";

  file_.open(str);

  logging_thread_ = std::jthread(std::bind_front(&LogUtils::MainLogger, this));
  LogInfo("Logger", "Started Logger");
}

LogUtils::~LogUtils() { logging_thread_.request_stop(); }

void LogUtils::MainLogger(std::stop_token stoken) {
  std::string print_output = "";
  while (!stoken.stop_requested()) {
    if (!logs_cache_.empty()) {
      LogData log = logs_cache_.front();
      logs_cache_.pop_front();

      auto time_t = std::chrono::system_clock::to_time_t(log.time_);
      std::tm tm = *std::localtime(&time_t);
      std::ostringstream oss;
      oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
      std::string timestamp = oss.str();

      std::string str = "";
      std::string message_severity = "";

      if (log.type_ == 0x01) {
        message_severity = "INFO";
      } else if (log.type_ == 0x03) {
        message_severity = "WARNING";
      } else if (log.type_ == 0x02) {
        message_severity = "ERROR";
      } else if (log.type_ == 0x00) {
        message_severity = "DEBUG";
      }

      str = FormatMessage(message_severity, log.r_time_, timestamp,
                          log.subtype_, log.message_);
      print_output += str + "\n";

      if (print_output.size() > kPrintLimit) {
        printf("%s", print_output.c_str());
        file_ << print_output;
        print_output.clear();
      }
    } else {
      if (print_output.size() != 0) {
        printf("%s", print_output.c_str());
        file_ << print_output;
        print_output.clear();
      }

      std::unique_lock<std::mutex> lock{mutex_};
      cv_.wait_for(lock, std::chrono::milliseconds(100), [this, &stoken] {
        return !logs_cache_.empty() || stoken.stop_requested();
      });
      if (stoken.stop_requested()) break;
      logs_cache_ = std::move(logs_);
      logs_.clear();
    }
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
  // throw std::runtime_error(subtype + " - " + message);
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

  std::string formated_string = FormatString(message, args);

  va_end(args);

  LogData log;
  log.type_ = LOG_TYPE_DEBUG;
  log.message_ = formated_string;
  log.time_ = std::chrono::system_clock::now();
  log.subtype_ = subtype;
  log.r_time_ =
      (std::chrono::high_resolution_clock::now() - init_time_).count();
  std::lock_guard<std::mutex> lock{mutex_};
  logs_.emplace_back(log);
  cv_.notify_one();
}

std::string LogUtils::FormatString(std::string in, ...) {
  va_list args;
  va_start(args, in);

  vsnprintf(buffer_.data(), kBufferSize, in.c_str(), args);

  va_end(args);

  std::string out(buffer_.data());
  memset(buffer_.data(), 0, out.size());
  return out;
}

std::string LogUtils::FormatMessage(std::string severity, int64_t time,
                                    std::string timestamp, std::string subtype,
                                    std::string message) {
  return FormatString("[ %lld NS ] [ %s ] [ %s / %s ]: %s", time,
                      timestamp.c_str(), severity.c_str(), subtype.c_str(),
                      message.c_str());
}
