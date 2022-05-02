// Condition variable is associated with event/condition, whereby one or more threads can WAIT for that condition to be satisfied.
// When condition is satisfied, thread wakes up one or more threads waiting on this condition

std::mutex m;
std::queue<dataChunk> dataQueue;
std::condition_variable dataCondition;

void dataPreparationThread() {
    while (moreDataToPrepare()) {
        dataChunk const data = prepareData();
        // Extra scoping just for clarity
        {
            std::lock_guard<std::mutex> lk(m);
            dataQueue.push(data);
        }
        dataCondition.notify_one();
    }
}

void dataProcessingThread() {
    while (true) {
        // use unique_lock over lock_guard
        // unique_lock can be locked and unlocked as needed, but lock_guard is only locked once on construction and unlocked on destruction
        std::unique_lock<std::mutex> lk(m);

        // if condition satisfied, execution continues into the lambda function (with the mutex still being locked)
        // if condition not satisfied, wait() unlocks the mutex and puts thread in BLOCK/WAITING state until notify_one is called again.
        // just before the condition is checked again, the mutex is reacquired 
        dataCondition.wait(
            lk, [] {return !dataQueue.empty(); }); 

    }

    // During a call to wait, a condition variable
    // 1. may check the supplied condition any number of times (use lambda function to prevent side-effects)
    // 2. Checks the condition with the mutex locked
    // 3. Returns immediately iff the function provided to test the condition returns true

    // Spurious wake arises when the waking thread acquires the mutex and checks the condition, but NOT in response to notify_one
}
