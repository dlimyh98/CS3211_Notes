#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x() {
    x.store(true, std::memory_order_release);
}

void write_y() {
    y.store(true, std::memory_order_release);     
}

void read_x_then_y() {
    while (!x.load(std::memory_order_acquire));    // guaranteed to synchronize with x.store(), order not guaranteed

    if (y.load(std::memory_order_acquire))         // guaranteed to synchronize with y.store(), order not guaranteed
        ++z;

    // because x.store() and y.store() are not synchronized
    // after x.load() is TRUE, it is not guarantee that y.load() is TRUE (y.store() has not run yet)
}

void read_y_then_x() {
    while (!y.load(std::memory_order_acquire)); 

    if (x.load(std::memory_order_acquire))   
        ++z;
}

int main() {
    x = false;
    y = false;
    z = 0;

    std::thread a{ write_x };
    std::thread b{ write_y };
    std::thread c{ read_x_then_y};
    std::thread d{ read_y_then_x};

    a.join();
    b.join();
    c.join();
    d.join();

    // Assert may return false
    assert(z.load() != 0);
}
