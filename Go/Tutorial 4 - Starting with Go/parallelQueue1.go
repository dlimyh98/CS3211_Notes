// only ONE sumCh channel to store sum, so that data can only be owned by one consumer at any time (to prevent data races)
// instead of storing one sum used by consumer SEQUENTIALLY, we can store many instances of sum on ALL consumers
func consumer(done chan struct{}, q <-chan int, sumCh chan int) {
    sum := 0    // local sum variable for each consumer

    for {
        select {
        case num := <-q:
            sum += num
        case <-done:
                sumCh <- sum
                return
        }
    }
}

// func producer() is same as before



func main() {
    done := make(chan struct{})
    q, sumCh := make(chan int), make(chan int)

    for i := 0; i < 10; i++ {
        go producer(done,q)
    }

    for j := 0; j < 5; j++ {
        go consumer(done,q,sumCh)
    }

    time.Sleep(2 * time.Second)
    close(done)

    // send LOCAL sum from all consumer channels back to main() for incrementation
    // main() use for-range loop to read from sumCh channel until channel is CLOSED
    sum := 0
    for subSum := range sumCh {
        sum += <-sumCh
    }
    // PROBLEM = channel can only be closed exactly once. who is going to make sure the sumCh is closed EXACTLY once?
    // As there are N writers to sumCh, we want to ensure that the last writer closes this channel (ask main() to read N signals from writers before closing the channel
    // 1. We could just read from sumCh N times, that will solve the issue
    // 2. Alternatively, every consumer could have it's own sumCh that it is responsible for closing. Then in main(), we simply loop over a vector of sumChs
    fmt.Println("Sum: ", sum)
}
