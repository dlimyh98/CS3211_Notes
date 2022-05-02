#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

// Thread A
void write_x_then_y() {
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_release);
}


// Thread B
void read_y_then_x() {
    while (!y.load(std::memory_order_acquire));

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
    assert(z.load() != 0);
}

/* Notepad Analogy */
/* 1. A calls notepad X, asking to write down a TRUE value as part of batch 1 (relaxed load)
*  2. A calls notepad Y, asking to write down a TRUE value as LAST value of batch 1 (store-RELEASE load)
*  3. In the meantime, B keeps calling notepad Y, asking for a value with batch information
*     He will be repeatedly rejected and keep trying, until one such entry comes in (provided by #2)
*  4. B will get the last value in batch 1 (TRUE), as well as who it is from
*  5. B then calls notepad X, and since he has batch information from #4, Acquire-Release semantics kick in
*  6. B tells X's notepad ALL of his batch information, and asks for last values in ANY of the batches
*  7. B therefore gets TRUE as part of batch 1 (from #1)
*/
