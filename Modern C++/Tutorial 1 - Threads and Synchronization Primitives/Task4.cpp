// Build a blocking dequeue using a Moniter
// A moniter consists of the following parts...
// 1. a Mutex
// 2. a Condition Variable
// 3. a Condition to wait for

#include <queue>
#include <mutex>
#include <condition_variable>

struct Job {
    int id;
    int data;
};

class JobQueue4 {
    std::queue<Job> jobs;
    std::mutex mut;      
    std::condition_variable conditionVar;

public:
    JobQueue4() : jobs{}, mut{} {}

    void enqueue(Job job) {
    std:: scoped_lock lock(mut);
        jobs.push(job);
        conditionVar.notify_one();     // notify all threads waiting on this condition (cannot specify order of threads to wake up)
    }

    Job dequeue() {
        std:: unique_lock lock(mut);   // only unique_lock works with Condition Variable. Mutex must be same as enqueue
        while (jobs.empty()) {         // checks the condition
            conditionVar.wait(lock);
            // how conditionVar.wait(lock) works....
            // automatically unlocks the Mutex and start waiting on the Condition Variable
            // when Condition Variable is notified (or spurious wakeup occurs), lock will be REACQUIRED before continuing
        }

        /* alternative implementation for lines 32-37 (this refers to the class that called the lambda function)
        *  conditionVar.wait(lock, [this]() {return !jobs.empty();});
        */

        Job job = jobs.front();
        jobs.pop();
        return Job{ -1, -1 };
    }
};
