#include "Utils/LogUtils.h"

#include <array>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <format>
#include <functional>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <stop_token>
#include <string_view>
#include <utility>

#include "FileManager/Files.h"

static constexpr std::array<std::string_view, 4> kSeverityStrings = {
    "DEBUG", "INFO", "ERROR", "WARNING"};

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
  std::string print_output;
  print_output.reserve(kPrintLimit + 512);

  char timestamp_buffer[32];

  while (!stoken.stop_requested()) {
    if (!logs_cache_.empty()) {
      LogData& log = logs_cache_.front();

      auto time_t = std::chrono::system_clock::to_time_t(log.time_);
      std::tm tm = *std::localtime(&time_t);
      std::strftime(timestamp_buffer, sizeof(timestamp_buffer),
                    "%Y-%m-%d %H:%M:%S", &tm);

      std::string_view severity =
          kSeverityStrings[static_cast<size_t>(log.type_)];

      print_output +=
          std::format("[ {} ] [ {} ] [ {} / {}]: {}\n", log.r_time_,
                      timestamp_buffer, severity, log.func_, log.message_);

      logs_cache_.pop_front();

      if (print_output.size() > kPrintLimit) {
        fputs(print_output.c_str(), stdout);
        file_ << print_output;
        print_output.clear();
      }
    } else {
      if (!print_output.empty()) {
        fputs(print_output.c_str(), stdout);
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

void LogUtils::Log(LogType type, std::string_view func, std::string&& msg) {
  LogData log;
  log.type_ = type;
  log.message_ = std::move(msg);
  log.time_ = std::chrono::system_clock::now();
  log.func_ = func;
  log.r_time_ =
      (std::chrono::high_resolution_clock::now() - start_time_).count();
  std::lock_guard<std::mutex> lock{mutex_};
  logs_.emplace_back(std::move(log));
  cv_.notify_one();
}
