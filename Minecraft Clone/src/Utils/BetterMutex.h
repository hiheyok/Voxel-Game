#pragma once
#include <atomic>
#define _CRTDBG_MAP_ALLOC
class atomicmutex {
    std::atomic<bool> flag{ false };

public:
    void lock()
    {
        while (flag.exchange(true, std::memory_order_relaxed));
        std::atomic_thread_fence(std::memory_order_acquire);
    }

    void unlock()
    {
        std::atomic_thread_fence(std::memory_order_release);
        flag.store(false, std::memory_order_relaxed);
    }
};

//https://stackoverflow.com/questions/15056237/which-is-more-efficient-basic-mutex-lock-or-atomic-integer#:~:text=atomic%20integer%20is%20a%20user,which%20runs%20in%20kernel%20mode.