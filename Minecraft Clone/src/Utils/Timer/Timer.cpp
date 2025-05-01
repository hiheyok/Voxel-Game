#include "Utils/Timer/Timer.h"

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

double Timer::GetTimePassed_μs() const noexcept {
  return std::chrono::duration<double, std::chrono::microseconds::period>(
             std::chrono::high_resolution_clock::now() - first_)
      .count();
}

double Timer::GetTimePassed_ns() const noexcept {
  return std::chrono::duration<double, std::chrono::nanoseconds::period>(
             std::chrono::high_resolution_clock::now() - first_)
      .count();
}

std::string Timer::StrGetTimePassed_s() const {
  return std::to_string(GetTimePassed_s());
}

std::string Timer::StrGetTimePassed_ms() const {
  return std::to_string(GetTimePassed_ms());
}

std::string Timer::StrGetTimePassed_μs() const {
  return std::to_string(GetTimePassed_μs());
}

std::string Timer::StrGetTimePassed_ns() const {
  return std::to_string(GetTimePassed_ns());
}
