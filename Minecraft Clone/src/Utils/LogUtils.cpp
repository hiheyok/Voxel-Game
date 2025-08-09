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
  Log(LogType::kInfo, FUNC_SIGNATURE, "Started Logger");
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

      if (log.type_ == LogType::kInfo) {
        message_severity = "INFO";
      } else if (log.type_ == LogType::kWarn) {
        message_severity = "WARNING";
      } else if (log.type_ == LogType::kError) {
        message_severity = "ERROR";
      } else if (log.type_ == LogType::kDebug) {
        message_severity = "DEBUG";
      }

      // "[ %lld NS ] [ %s ] [ %s / %s ]: %s"

      std::string_view fmt = "[ {} ] [ {} ] [ {} / {}]: {}";

      int64_t ns_time = log.r_time_;
      std::string date = timestamp;
      std::string_view func = log.func_;
      std::string msg = log.message_;
      std::string severity = message_severity;

      str = std::vformat(
          fmt, std::make_format_args(ns_time, date, severity, func, msg));

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

void LogUtils::Log(LogType type, std::string_view func, std::string msg) {
  LogData log;
  log.type_ = type;
  log.message_ = msg;
  log.time_ = std::chrono::system_clock::now();
  log.func_ = func;
  log.r_time_ =
      (std::chrono::high_resolution_clock::now() - start_time_).count();
  std::lock_guard<std::mutex> lock{mutex_};
  logs_.emplace_back(log);
  cv_.notify_one();
}
