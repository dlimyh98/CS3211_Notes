/* Designing for Concurrency */
/* 1. Multiple threads can access the data structure concurrently (threads may be performing same or distinct operations)
*  2. Each thread sees a self-CONSISTENT view of the data structure
*/

/* Designing for Thread-Safe data structures */
/* 1. No data is lost or corrupted
*  2. All invariants are upheld
*     - no thread can see a state where the invariants of the data structure have been broken by actions of ANOTHER thread
*     - pay attention to how data structure behaves in presence of exceptions, to ensure invariants hold too
*  3. No problematic race conditions
*     - provide functions for COMPLETE operations (atomic operations), rather than for operation steps
*     - minimize opportunity for deadlocks by restricting scope of locks and avoiding nested locks
*  4. Misc
*     - Multiple threads might perform one type of operation concurrently, another operations requires exclusive access by a single thread
*       use std::shared_mutex to allow concurrent reads, but exclusive access for writes
*     - Safe for multiple threads to access data structure concurrently, if they are performing DIFFERENT actions
*     - Constructors and Destructors require exclusive access to data structure 
*       up to users to ensure data structure not accessed before construction is complete, or after destruction has started
*     - Assignment, swap(), Copy Construction
*       decide whether it's safe for these operations to be called concurrently with other operations, or whether they require exclusive access
*/

#include <exception>

// creating user-defined Exception class
struct emptyStack : std::exception {
    const char* what() const throw();
    // const throw ()
    // - const means that this function will not change observable state of the non-mutable memebers of the object it is called on.
    // - hence it is able to be called on const instances of the class
    // throw() means you promise to the compiler that function will never allow exception to be emitted (depreciated)
};

template <typename T>
class threadSafeStack {
private:
    std::stack<T> data;
    mutable std::mutex m;    // mutex is able to be modified by a const function
public:
    // Constructors
    threadSafeStack() {}
    threadSafeStack(const threadSafeStack& other) {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }

    // Copy Assignment operator is deleted (stack is not able to be copied)
    threadSafeStack& operator=(const threadSafeStack&) = delete;

    void push(T newValue) {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(newValue));      // use move constructor to take ownership of newValue directly, do not pass reference!
    }

    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);

        if (data.empty())
            throw emptyStack();

        std::shared_ptr<T> const res(
            std::make_shared<T>(std::move(data.top())) // using std::make_shared is more efficient than calling std::shared_ptr constructor
        );

        data.pop();
        return res;
    }

    // isEmpty const function is unable to change a (non-mutable) member variable of the class
    // however the mutex is declared mutable, so it can still be changed by lock_guard inside const isEmpty function!
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};