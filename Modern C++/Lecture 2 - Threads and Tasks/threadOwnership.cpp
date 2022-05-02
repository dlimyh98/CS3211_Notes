// Thread has same ownership semantics as std::unique_ptr
// std::thread instances own the resource, and are movable but not copyable
// std::thread can only have at most one owner


/* Thread Ownerships */
void functionA();
void functionB();
std::thread t1(functionA);           // initialize a thread such that t1 points to it
std::thread t2 = std::move(t1);      // t2 now points to what t1 was pointing. t1 is now a dangling pointer
t1 = std::thread(functionB);         // reassign t1 to some other thread
t1 = std::move(t2);                  // error, t1 cannot point to multiple threads


/* Transferring ownership of Threads (out of function that returns a thread) */

// Example 1
std::thread f() {
    void someFunction();
    return std::thread(someFunction);
}

// Example 2
std::thread g() {
    void someOtherFunction();
    std::thread t(someOtherFunction(), 42);
    return t;
}


/* Transfer ownership of Threads (into a function) */

// Example 1
void f(std::thread t);              // does something with a thread

// Example 2
void g() {
    void someFunction();
    f(std::thread(someFunction));   // passing in a Thread that runs some function
}
