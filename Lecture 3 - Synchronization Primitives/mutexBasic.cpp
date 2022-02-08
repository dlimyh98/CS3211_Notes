// global declaration of Mutex
std::mutex someMutex;

// use lock_guard since it has RAII capabilities
void addToList(int newValue) {
    std::lock_guard<std::mutex> guard(some_mutex);
    // alternatively, lock_guard has template argument deduction
    // so we can use std::lock_guard guard{some_mutex}; instead.
    // note the curly braces, normal braces will just be a function declaration
    someList.push_back(newValue);
}
