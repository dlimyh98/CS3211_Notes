#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

// Thread A
void write_x_then_y() {
    // all non-atomic and relaxed atomic stores BEFORE releaseFence in A will happen-before
    // all non-atomic and relaxed atomic stores from SAME LOCATION (variable) made in B, AFTER acquireFence
    x.store(true, std::memory_order_relaxed);

    std::atomic_thread_fence(std::memory_order_release);    // releaseFence synchronizes with acquireFence
    y.store(true, std::memory_order_relaxed);
}


// Thread B
void read_y_then_x() {
    while (!y.load(std::memory_order_relaxed));
    std::atomic_thread_fence(std::memory_order_acquire);    // acquireFence synchronizes with releaseFence

    // x.store in line 12 happens-before x.load in line 26
    if (x.load(std::memory_order_relaxed))
        ++z;
}

int main() {
    x = false;
    y = false;
    z = 0;

    std::thread a{ write_x_then_y };
    std::thread b{ read_y_then_x };

    a.join();
    b.join();

    // Assert always returns true
    // if line 10 and 11 swapped, assert may return false
    assert(z.load() != 0);
}
