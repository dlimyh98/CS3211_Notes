// Implement mutexes so that queue can be used concurrently
// - instead of locking job queue as a whole, use a fine-grained approach
// - allow producers and consumers to not only operate concurrently, but in parallel (as long as queue is large enough, so no contention on nodes)

// - optimize the use case for 
// 1. ONE producer
// 2. ONE consumer
// 3. enough elements in the queue lined up
// then the producer will not block the consumer from accessing the queue concurrently
// if multiple producers/consumers, producers are allowed to block other producers, consumers allowed to block other consumers

#include <new>
#include <optional>
#include <thread>
#include <mutex>

struct Job {
    int id;
    int data;
};

class JobQueue5 {
    // A node can be a dummy node (contains no jobs and next == nullptr) or
    // it can be a regular node (contains a job and next != nullptr)
    // The last node in the queue (node at producer end) is always the dummy
    // node, all other nodes are regular nodes
    struct Node {
        // TODO
        Node* next = nullptr;
        Job job{};             // default initialization {0,0}
    };

    Node* jobs_back;   // producer end
    Node* jobs_front;  // consumer end
    std::mutex producerMutex;
    std::mutex consumerMutex;
    std::mutex consumerProducerMutex;

public:
    // Queue starts with a dummy node
    JobQueue5() : jobs_back{ new Node{} }, jobs_front{ jobs_back } {}

    ~JobQueue5() {
        // Assumption: no other threads are accessing the job queue
        while (jobs_front != nullptr) {
            Node* next = jobs_front->next;
            delete jobs_front;
            jobs_front = next;
        }
    }

    /* possible Data Races */
    // 1.  Producer-Producer data race
    // 2.  Consumer-Consumer data race
    // 3.  Producer-Consumer data race (when there is an empty list at first; Producer tries to enqueue and Consumer tries to dequeue)
    // 3.1 Producer writes to jobs_back (jobs_back->next = new_node)
    // 3.2 Consumer reads from jobs_front (old_node->next == nullptr)
    // 3.3 Both memory regions are the same (jobs_back == jobs_front), hence data race
    //     disregard the amount of time that happens between 3.1 and 3.2. As long as they are not synchronized, data race will happen


    void enqueue(Job job) {
        std::unique_lock lockProd(producerMutex);
        // Make a new dummy node
        Node* new_node = new Node{};

        // Turn old dummy node into regular node
        std::unique_lock lockProdCon(consumerProducerMutex);    // to prevent Producer-Consumer datarace
        jobs_back->job = job;
        jobs_back->next = new_node;        // data race (Producer and Consumer)
        jobs_back = new_node;
    }

    std::optional<Job> try_dequeue() {
        std::unique_lock lockCon(consumerMutex);
        // TODO
        Node* old_node;
        {
            old_node = jobs_front;

            std::unique_lock lockProdCon(consumerProducerMutex);  // to prevent Producer-Consumer datarace
            if (old_node->next == nullptr) {     // data race (Producer and Consumer)
                // Node was dummy, so the queue is empty
                return std::nullopt;
            }

            // Node was not dummy, delete node and return job
            jobs_front = jobs_front->next;
        }

        Job job = old_node->job;
        delete old_node;

        return job;
    }
};