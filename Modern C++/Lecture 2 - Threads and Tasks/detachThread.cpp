void doSomething(int& i) {
    ++i;
}

struct funct {
    int& i;
    funct(int& i_) :(i_) {}

    void operator()() {
        for (int i = 0; i < 10; i++) {
            doSomething(i);
        }
    }
};

// Detaches thread, allowing them to run independently
// Local variables passed as parameters to threads might end their lifetime, before the thread can finish execution
void oops() {
    int localState = 0;
    funct myFunct(localState);
    std::thread myThread(myFunct);
    myThread.detach();
}

// main thread and myThread get detached
// main thread exits oops function, and destroys localState variable
// myThread still continues to execute, but now accesses a reference to localState that is undefined
// think of reference as constant dereferenced pointer!
int main() {
    oops();
}