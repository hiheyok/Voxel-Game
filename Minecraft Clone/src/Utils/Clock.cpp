#include <chrono>
#include "Clock.h"
#include <windows.h>    /* WinAPI */
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace chrono;


void precisePause(double nanoseconds) {
    double nanot_ = 0;
    auto t0 = std::chrono::high_resolution_clock::now();

    while (nanot_ <= nanoseconds) {
        std::chrono::duration<double, std::nano> nano = std::chrono::high_resolution_clock::now() - t0;
        nanot_ = nano.count();
    }
}

void timerSleep(double seconds) {
    using namespace std;
    using namespace std::chrono;

    static double estimate = 2e-3;
    static double mean = 2e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds > estimate) {
        auto start = high_resolution_clock::now();
        timeBeginPeriod(1);
        this_thread::sleep_for(microseconds(1));
        timeEndPeriod(1);
        auto end = high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2 += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = high_resolution_clock::now();
    while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
}

void timerSleepNotPrecise(int milliseconds) {
    timeBeginPeriod(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    timeEndPeriod(1);
}