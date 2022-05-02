/* Passing a function along with the function's arguments (by value) to thread */
void function(int i, std::string const& s);

std::thread myThread(function, 3, "hello");


/* Casting arguments before passing to thread */
void function(int i, std::string const& s);

void oops(int someParameter) {
    char buffer[1024];
    sprintf(buffer, "%i", someParameter);

    // oops might exit (thus destroying local variable buffer) before the buffer is converted from char[] to std::string
    // Conversion is as follows
    // 1. myThread is created, starts executing
    // 2. function starts executing
    // 3. when the function parses it's arguments, THEN the conversion starts executing.
    // by then, buffer may not exists anymore, and it is undefined behaviour
    std::thread myThread(function, 3, buffer);

    // thread might get detached before the argument is fully converted
    // we might be unable to successfully call f, as the buffer dissapears before f can be successfully called (we exit oops function)
    myThread.detach();

    // a simple fix to the above issue, is to cast the argument beforehand
    // this way, the conversion happens BEFORE the argument can be passed to the thread (and the thread is started)
    std::thread myThread(function, 3, std::string(buffer));
}


/* Passing arguments by reference */
void updateData(widgetID w, widgetData& data);

void oops(widgetID w) {
    widgetData data;
    std::thread myThread(updateData, w, data);             // std::thread constructor copies supplied value without converting to expected argument type (which is a reference)
    std::thread myThread(updateData, w, std::ref(data));   // simple fix is to wrap arguments 
}
