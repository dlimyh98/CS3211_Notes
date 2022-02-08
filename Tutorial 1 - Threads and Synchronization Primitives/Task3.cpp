#include <queue>
#include <mutex>
#include <semaphore>

// in Task 2, if queue was empty, consumer has no means to block until a new element arrives
// use Counting Semaphore to modify queue, such that dequeue blocks until the queue is non-empty

struct Job {
    int id;
    int data;
};

// Unbounded queue with enqueue, non-blocking and blocking dequeue
// Use semaphores to signal blocked consumers
class JobQueue3 {
    std::queue<Job> jobs;
    std::mutex mut;      
    std::counting_semaphore<> count;

public:
    // count corresponds to how many jobs in queue
    JobQueue3() : jobs{}, mut{}, count{ 0 } {}

    void enqueue(Job job) {
    std:: scoped_lock lock(mut);
        jobs.push(job);
        count.release();
    }

    // try_dequeue function doesn't block if there is nothing to dequeue
    std::optional<Job> try_dequeue() {
        if (count.try_acquire()) {         // try to acquire Semaphore, WITHOUT blocking
            std:: scoped_lock lock(mut);
            Job job = jobs.front();
            jobs.pop();
        }
        else {
            return std::nullopt;
        }
    }

    // dequeue function blocks if there is nothing to dequeue
    Job dequeue() {
        count.acquire();                 // tries to acquire. If it fails, BLOCKS until it can
        std:: scoped_lock lock(mut);
        Job job = jobs.front();
        jobs.pop();
        return Job{ -1, -1 };
    }
};