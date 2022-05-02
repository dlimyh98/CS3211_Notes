#include <iostream>
#include <thread>
#include <mutex>

int counter;
std::mutex counterMutex;

int main() {
    std::thread t0{ []() {
        std::lock_guard<std::mutex> lock(counterMutex);
        ++counter; } };

    std::thread t1{ []() { 
        std::lock_guard<std::mutex> lock(counterMutex);
        ++counter; } };

    t0.join();
    t1.join();

    std::cout << counter << std::endl;

    return 0;
}

// lock_guard ensures that no data race
