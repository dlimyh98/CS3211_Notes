#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x() {
    x.store(true, std::memory_order_seq_cst);
}

void write_y() {
    y.store(true, std::memory_order_seq_cst);
}

void read_x_then_y() {
    while (!x.load(std::memory_order_seq_cst));    // keep reading until x is written to

    if (y.load(std::memory_order_seq_cst))         // if y has been written to
        ++z;
}

void read_y_then_x() {
    while (!y.load(std::memory_order_seq_cst));   // keep reading until y is written to

    if (x.load(std::memory_order_seq_cst))        // if x has been written to
        ++z;
}

int main() {
    x = false;
    y = false;
    z = 0;

    // If every atomic that uses seq_cst ALWAYS uses seq_cst (which is recommended), then the rule is obvious:
    // Interleave the different threads together (coherence ordered-before) WHILE obeying Program Order (strongly happens-before)

    // Sequentially Ordered, so ALL threads see the same sequence of operations on x and y
    std::thread a{ write_x };
    std::thread b{ write_y };
    std::thread c{ read_x_then_y};
    std::thread d{ read_y_then_x};

    a.join();
    b.join();
    c.join();
    d.join();

    // Assert always true
    assert(z.load() != 0);
}

/* Analysis of program */
/* 1. Either line 8 (write_x) or line 12 (write_y) happened first
*  2.1 If line 17 returns false (y.load is false), means that line 8 (write_x) happened first
*  2.2 If line 23 returns false (x.load is false), means that line 12 (write_y) happened first
*  
*  So even if one thread doesn't increment z, the other thread will.
*  This is because eventually, the changes in x and y have to be seen in the SAME ORDER through ALL threads.
*/
