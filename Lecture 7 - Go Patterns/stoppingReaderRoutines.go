// doWork function (reader goRoutine)
doWork := func(
    done <-chan interface{},                       // input channel of any type
    strings <-chan string,                         // input channel of string type
) <-chan interface{} {                             // return type is channel interface{}
    terminated := make(chan interface{})

    go func() {
        defer fmt.Println("doWork exited.")        // deferred until goFunc() returns
        defer close(terminated)                    // deferred until goFunc() returns
        for {
            select {
            case s := <-strings:
                // Do something with the strings
            case <-done:                           // provides us with an "escape", goRoutine able to close itself if done channel is written to
                return                             // goFunc() returns here, when able to read from done channel
            }
        }
    }()
    return terminated                              // returns terminated channel to calling main() function
}

// some portion of main code (stops the reader goRoutine)
done := make(chan interface{})
terminated := doWork(done,nil)
go func() {
    // Cancel the operation after 1 second
    time.Sleep(1 * time.Second)
    fmt.Println("Cancelling doWork goRoutine...")
    close(done)                                    // will cause doWork() to close
}()
<-terminated                                       // BLOCKING read from terminated channel
fmt.Println("Done.")
