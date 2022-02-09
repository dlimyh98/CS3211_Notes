#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

// Thread A
void write_x_then_y() {
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_relaxed);

    // WITHIN this PARTICULAR thread, x.store guaranteed to happen before y.store (Program Order)
    // but OTHER threads may not see this ordering
}

/* No happens-before between STORES and LOADS */

// Thread B
void read_y_then_x() {
    while (!y.load(std::memory_order_relaxed));

    if (x.load(std::memory_order_relaxed))
        ++z;

    // WITHIN this PARTICULAR thread, y.load is guaranteed to happen before x.load (Program Order)
    // but OTHER threads may not see this ordering
}

int main() {
    x = false;
    y = false;
    z = 0;

    std::thread a{ write_x_then_y };
    std::thread b{ read_y_then_x };

    a.join();
    b.join();

    // Assert may return false
    assert(z.load() != 0);
}

/* Analysis of program */
/* 1. Within write_x_then-y, guaranteed that x.store happens before y.store
*  2. Within read_y_then_x, guaranteed that y.load happens before x.load
*  3. Relaxed ordering only ensures ALL threads agree on modification order for SAME variable, no guarantee of how it will be ordered like
*  4. An analogy....
*  4.1 You and your friend both agree that x = false, y= false
*  4.2 You send him a letter saying that x = true.
*  4.3 Next day, you send him another letter saying y = true.
*  4.4 You know that DEFINITELY, you sent x before y
*  4.5 However, your friend may receive letter y first (x mail got delayed)
*  4.6 Hence from his POV, y = true but x = false
*  4.7 Similarly, memory between threads has NO GUARANTEE at all that writes from other threads turn up in any particular order.
*      That is what STRONGER ordering is for.
*/

/* Notepad Analogy */
/* 1. A calls notepad X, asks to write down TRUE (which will be written to the bottom of X's list)
*  2. A calls notepad Y, asks to write down TRUE (which will be written to the bottom of Y's list)
*  3. In the meantime, B keeps calling notepad Y, asking for any value on the list
*     He will be repeatedly rejected and keep trying, until A writes down onto notepad Y (as per #2)
*  4. B gets any value on Y's notepad
*  5. B then continues to call notepad X. Since it is his first time calling, he COULD get x = false
*     (even though X has been written to be TRUE by A)
*  6. Hence assert may return false
*/
