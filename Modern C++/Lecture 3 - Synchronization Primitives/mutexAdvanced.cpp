class someData {
    int a;
    std::string b;

public:
    void doSomething();
};


// group Mutex and protectedData together in class (OOP)
class dataWrapper {
private:
    someData data;
    std::mutex m;

public:
    template <typename Function>
    void processData(Function func) {
        std::lock_guard<std::mutex> l(m);
        func(data);
    }
};

someData* unprotectedData;

// Pass in a reference to someData
void maliciousFunction(someData& protectedData) {
    unprotectedData = &protectedData;    // Get the address of protectedData, and pass it to global variable unprotectedData (a pointer)
}

dataWrapper wrap;

// Here, maliciousFunction is able to bypass the lock and access the protectedData
// To fix this, never pass pointers/references to protectedData outside of the scope of the lock.
void foo() {
    wrap.processData(maliciousFunction);
    unprotectedData->doSomething();
}
