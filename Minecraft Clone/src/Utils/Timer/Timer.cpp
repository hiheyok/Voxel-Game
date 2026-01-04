#include "Utils/Timer/Timer.h"

#include <chrono>
#include <string>

using std::string;
using std::to_string;

Timer::Timer() noexcept : first_{std::chrono::high_resolution_clock::now()} {}

void Timer::Set() { first_ = std::chrono::high_resolution_clock::now(); }

double Timer::GetTimePassed_s() const noexcept {
  return std::chrono::duration<double>(
             std::chrono::high_resolution_clock::now() - first_)
      .count();
}

double Timer::GetTimePassed_ms() const noexcept {
  return std::chrono::duration<double, std::chrono::milliseconds::period>(
             std::chrono::high_resolution_clock::now() - first_)
      .count();
}

double Timer::GetTimePassed_us() const noexcept {
  return std::chrono::duration<double, std::chrono::microseconds::period>(
             std::chrono::high_resolution_clock::now() - first_)
      .count();
}

double Timer::GetTimePassed_ns() const noexcept {
  return std::chrono::duration<double, std::chrono::nanoseconds::period>(
             std::chrono::high_resolution_clock::now() - first_)
      .count();
}

string Timer::StrGetTimePassed_s() const {
  return to_string(GetTimePassed_s());
}

string Timer::StrGetTimePassed_ms() const {
  return to_string(GetTimePassed_ms());
}

string Timer::StrGetTimePassed_us() const {
  return to_string(GetTimePassed_us());
}

string Timer::StrGetTimePassed_ns() const {
  return to_string(GetTimePassed_ns());
}
