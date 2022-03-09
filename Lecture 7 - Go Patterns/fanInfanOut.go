// Fan-Out = start multiple goRoutines to handle input from pipeline
// 1. No guarantee on the order that concurrent goRoutines will run on
// 2. Naive implementation of fan-out works only if order of results is UNIMPORTANT
numFinders := runtime.NumCPU()
finders := make([]<-chan int, numFinders)

for i := 0; i < numFinders; i++ {
    finders[i] = primeFinder.(done, randIntStream)
}



// Fan-In = combine multiple results into one channel
// Involves multiplexing/joining together MULTIPLE streams of data into SINGLE stream
fanIn := func(
    done <-chan interface{},
    channels...<-chan interface{}
) <-chan interface{} {
    var wg sync.WaitGroup
    multiplexedStream := make(chan interface{})     // all channels will join their values into this SINGLE channel

    multiplex := func(c <-chan interface{}) {
        defer wg.Done()

        for i := range c {
            select {
            case <- done:
                return
            case multiplexedStream <- i:            // for some particular channel, it will push it's values onto the COMMON single channel
            }
        }
    }

    // add all channels to waitGroup
    wg.Add(len(channels))

    // for all channels, join their values down into ONE channel
    for _, c := range channels {
        go multiplex(c)
    }

    // wait for all reads to complete
    go func() {
        wg.Wait()
        close(multiplexedStream)
    }()

    return multiplexedStream
}
