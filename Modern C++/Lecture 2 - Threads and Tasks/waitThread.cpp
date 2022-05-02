void doSomething(int& i) {
    ++i;
}

struct funct {
    int& i;                       // Reference to int (constant dereferenced pointer in C++)

    funct(int& i_) :i(i_) {};     // Initializer List for Constructor

    void operator()() {
        for (int i = 0; i < 10; i++) {
            doSomething(i);
        }
    }
};

// Even if exception occurs in function, be sure to still join the thread
void function() {
    int localState = 0;
    funct myFunct(localState);    // localState is passed to constructor of myFunct
    std::thread t(myFunct);

    try {
        doSomethingInThread();
    }
    catch(...) {
        t.join();
        throw;
    }
    t.join();
}

int main() {
    function();
}