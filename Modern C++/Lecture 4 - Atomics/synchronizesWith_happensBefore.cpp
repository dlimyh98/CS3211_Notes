#include <vector>
#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>

std::vector<int> data;
std::atomic<bool> dataReady{false};

void readerThread() {
    while (!dataReady.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << "The answer = " << data[0] << "\n";
}

void writerThread() {
    data.push_back(42);
    dataReady = true;
}


/* Happens-before relationship */
/* A and B be operations in multithreaded program. 
   If A happens-before B, then memory effects of A are visible to thread performing B, BEFORE B is performed
* 
* 1. Intra-thread Happens-before (aka Program Order, WITHIN a thread)
* - if operation A occurs in a statement prior to operation B in the source code, means that A happens before B
* - there can be no Happens-before relationship between operations that occur in the SAME line
*   as there can be multiple instructions separated by comma, unclear which instruction to run first
* 
* 2. Inter-thread Happens-before (amongst DIFFERENT threads)
*    thread1
*    -------
*    0x10: int a = 1;
*    0x11: sendResultToThread2(a);
*    ------- 
* 
*    thread2
*    -------
*    0x12: int c = getResultFromThread1();
*    -------
*    assume no data races and atomicity for instructions...
* 
*    if 0x10 is sequenced before 0x11 (as it is, due to Program Order), and 0x11 happens before 0x12 (in a multi-threaded environment),
*    assuming there is some sort of mechanism for which memory effects done by 0x11 are visible to 0x12, before 0x12 executes
*    assuming 0x11 always executed before 0x12 in multi-threaded environment,
*    then 0x10 Inter-thread Happens-before 0x12.
* 
* 3. Transitivity holds
* - if A happens before B, and B happens before C, then A happens before C
*/
// line 18 happens before line 19
// line 11 (true state) happens before line 14



/* Synchronizes-with relationship */
/* Happens only between operations on atomic types, between different threadsa
* (synchronizes with) -> (happens-before)
* 
* A suitably tagged atomic WRITE operation (W) on a variable x, syncs with a suitably tagged atomic READ operation (R) on x
* R reads the value stored by x, and returns
* 1. either that WRITE W, or
* 2. a subsequent atomic WRITE operation on x by the SAME thread that performed the initial WRITE W
* 3. a sequence of atomic READ-MODIFY-WRITE operations on x (e.g. fetch_add() or compare_exchange_weak()) by ANY thread, where the 
*    value read by the first thread in the sequence is the value written by W
* 
* Synchronization doesn't mean that it MUST happen, it just means that the two points have been connected, and can facilitate something.
* e.g. if line 19 happens, it doesn't mean that program order immediately jumps to line 11.
*/
// line 19 sync with line 11
