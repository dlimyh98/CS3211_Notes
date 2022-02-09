#include <atomic>
#include <assert.h>

std::atomic<int> data[5];
std::atomic<bool> sync1(false), sync2(false);

// Modifies some stored(global) variables, does a store-RELEASE to one of them (sync1)
void thread_1()
{
    data[0].store(42, std::memory_order_relaxed);
    data[1].store(97, std::memory_order_relaxed);
    data[2].store(17, std::memory_order_relaxed);
    data[3].store(-141, std::memory_order_relaxed);
    data[4].store(2003, std::memory_order_relaxed);
    sync1.store(true, std::memory_order_release);    // store-RELEASE sync1 
}

// load-ACQUIRES the stored variable that was store-RELEASED in thread_1
// then performs a store-RELEASE on 2nd shared variable (sync2)
void thread_2()
{
    while (!sync1.load(std::memory_order_acquire));              
        sync2.store(true, std::memory_order_release);                
}

// does load-ACQUIRE on second shared variable (sync2) that was store-RELEASED in thread_2
void thread_3()
{
    while (!sync2.load(std::memory_order_acquire));             

    // all asserts below return true
    assert(data[0].load(std::memory_order_relaxed) == 42);
    assert(data[1].load(std::memory_order_relaxed) == 97);
    assert(data[2].load(std::memory_order_relaxed) == 17);
    assert(data[3].load(std::memory_order_relaxed) == -141);
    assert(data[4].load(std::memory_order_relaxed) == 2003);
}