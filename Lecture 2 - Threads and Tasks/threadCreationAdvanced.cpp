/* Thread with function (as per threadCreationSimple.cpp) */
void doWork();
std::thread(doWork);

/* Thread with a functor (can use class or struct) */
class backgroundTask {
public:
    void operator()() const {
        doSomething();
    }
};

backgroundTask funct;
std::thread myThread(funct);

/* Declare a function that returns a thread, NOT a thread with a functor! */
// function called my_thread that accepts functor, returns std::thread
std::thread myThread(backgroundTask());

/* Thread with a functor(using double brackets or list initialization) */
std::thread myThread((backgroundTask()));
std::thread myThread{backgroundTask()};

/* Thread with lambda expression */
// lambda expression is as follows ... [captureClause] (parameters) -> returnType {methodDefinition}
std::thread myThread(
    [] {
        doSomething();
    }
);
