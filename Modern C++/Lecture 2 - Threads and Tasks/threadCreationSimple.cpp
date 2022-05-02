#include <iostream>
#include <thread>

void hello() {
	std::cout << "Hello Concurrent World \n";
}

// Process has at least one execution thread when it starts
int main() {
	std::thread t(hello);	// Creates a thread named t, that runs hello function
	t.join();               // main thread that invoked new thread, will wait for new thread to finish execution before continuing.
}