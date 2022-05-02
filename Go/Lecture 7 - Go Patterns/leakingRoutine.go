// doWork()
doWork := func(strings <-chan string) <-chan interface{} {
    completed := make(chan interface{})
    go func() {
        defer fmt.Println("doWork exited.")
        defer close(completed)
        for s := range strings {
            // do some work
            fmt.Println(s)
        }
    }()

    return completed
}

// some part in main()...
doWork(nil)                              // reading from nil channel is BLOCKING
// main() will never progress here
// but goRoutines are still being spawned in doWork(), hence memory leak
fmt.Println("Done.")
