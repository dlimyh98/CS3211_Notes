#include <future>
#include <iostream>
#include <thread>
#include <atomic>

// This is a generic class where T can be any type.
// Don't worry too much about it.
template <typename T>
class simple_shared_ptr {
    T* ptr;
    // INCORRECT: std::size_t* count;
    std::atomic<std::size_t> *count;     // count (pointer) is not shared, but *count (dereferenced pointer) is

public:
    simple_shared_ptr(T* ptr) : ptr{ ptr }, count{ new std::size_t{1} } {
        std::cout << "Ptr ctor" << std::endl;
    }

    // Copy Constructor
    simple_shared_ptr(const simple_shared_ptr& other)
        : ptr{ other.ptr }, count{ other.count } {
        std::cout << "Copy ctor" << std::endl;
        ++* count;    // count is shared amongst all 
    }

    // Prevent simple_shared_ptr from being mutated after construction
    simple_shared_ptr& operator=(const simple_shared_ptr& other) = delete;

    T* get() {
        std::cout << "get" << std::endl;
        return ptr;
    }

    // what happens when two threads try to destroy the same shared_ptr?
    // if two threads try to decrement count, where count is in memory, then there is data race
    ~simple_shared_ptr() {
        std::cout << "Dtor" << std::endl;
        if (-- * count == 0) {
            delete ptr;
            delete count;
        }
    }
};

int main() {
    std::promise<simple_shared_ptr<int>> int_ptr_promise;
    std::future<simple_shared_ptr<int>> int_ptr_future =
        int_ptr_promise.get_future();

    std::thread t1{ [&int_ptr_promise]() {
        // Shared pointer is allocated here
        simple_shared_ptr<int> ptr{new int{5}};

        // Copy the pointer, and pass to thread t2
        int_ptr_promise.set_value(ptr);

        // Shared pointer is deallocated here
      } };

    std::thread t2{ [&int_ptr_future]() {
        // Obtain shared pointer from future
        simple_shared_ptr<int> ptr = int_ptr_future.get();

        // Use it
        std::cout << *ptr.get() << std::endl;

        // Shared pointer is deallocated here
      } };

    t1.join();
    t2.join();

    return 0;
}