#include <chrono>
#include <windows.h>    /* WinAPI */
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <immintrin.h> // For _mm_pause()
#include "Utils/Clock.h"



void precisePause(double nanoseconds) {
    double nanot_ = 0;
    auto t0 = std::chrono::high_resolution_clock::now();

    while (nanot_ <= nanoseconds) {
        std::chrono::duration<double, std::nano> nano = std::chrono::high_resolution_clock::now() - t0;
        nanot_ = nano.count();
    }
}

void timerSleep(double seconds) {
    using namespace std::chrono;

    if (seconds <= 0.0) return;

    constexpr double sleep_threshold = 2e-3; // 2 milliseconds
    auto start_time = high_resolution_clock::now();

    if (seconds > sleep_threshold) {
        auto sleep_duration = duration<double>(seconds - sleep_threshold);
        std::this_thread::sleep_for(sleep_duration);
    }

    // Spin-wait for the remaining time
    while (duration<double>(high_resolution_clock::now() - start_time).count() < seconds) {
        _mm_pause(); // Reduces CPU power consumption during spin-wait
    }
}

void timerSleepNotPrecise(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}