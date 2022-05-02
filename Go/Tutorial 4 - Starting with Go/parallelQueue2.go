func producer(done chan struct{}, q chan int) {
    for {
        select {
        case q <- 1:    // Repeatedly writing to q channel (if allowed to)
        case <-done:    // If done channel can be read from, we exit
            return
        }
    }
}

func consumer(done chan struct{}, q chan int, sumCh chan<-int) {
    sum := 0

    for {
        select {
        case <-done:          // if need to exit
            sumCh <- sum      // consumer sends it's LOCAL sum to IT'S OWN sumCh channel
            close(sumCh)      // CLOSE the sumCh channel
            return
        case num := <-q:
            sum += num
        }
    }
}

var (
    numProducer = 5
    numConsumer = 5
)

func main() {
    start, done := make(chan struct{}), make(chan struct{})
    q := make(chan int)

    // Create slice (C++ vector) of chan ints, initialize to 0 number of items, numConsumer is capacity of slice (but slice can auto resize)
    sumChs := make([]chan int, 0, numConsumer)
    for i := 0; i < numConsumer; i++ {
        sumCh := make(chan int, 1)          // make a channel
        sumChs = append(sumChs, sumCh)      // append this channel to vector
    }

    for i := 0; i < numProducer; i++ {
        go func() {
            <-start               // try to read from start channel (BLOCKING read)
            producer(done,q)      // if able to read, then start producing
        }()
    }

    for j := 0; j < numConsumer; j++ {
        j := j                                // capture current j value in scope (for closure)
        go func() {
            <-start                           // try to read from start channel (BLOCKING read)
            consumer(done,q,sumChs[j])        // if able to read, start consuming using the CURRENT j value (via CLOSURE)
        }()
    }

    close(start)              // signal to goRoutines to start
    time.Sleep(time.Second)   // sleep
    close(done)               // signal to goRoutines to end

    // collect all of the sums
    sum := 0
    for _, ch := range sumChs {
        sum += <-ch
    }

    fmt.Println("Sum: ", sum)
}
