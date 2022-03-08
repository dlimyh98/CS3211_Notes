func producer(done chan struct{}, q chan<-int) {
    for {
        select {        // wait on multiple channels using select
        case q <- 1:    // repeatedly write 1 to channel q (if it's ready to be written to)
        case <- done:   // if managed to read from done, then exit
            return
        }
    }
}

/* consumer is NOT parallel here, as use of sumCh channel is sequential */
func consumer(done chan struct{}, q <-chan int, sumCh chan int) {
    for {
        select {
        case num := <-q:              // read from channel q (if it's ready to be read from) and store into num
            select {
            case sum := <-sumCh:      // read from channel sumCh (if it's ready to be read from) and store into sum
                sumCh <- num + sum    // increment sum and send it back to channel sumCh (if it's ready to be written to)
            case <-done:              // if managed to read from done, then exit
                return
            }
        case <-done:                  // if managed to read from done, then exit
            return
        }
    }
}

func main() {
    done := make(chan struct{})                   // empty struct used, since we want to notify about some event but don't need to pass information about it
    q, sumCh := make(chan int), make(chan int)

    for i := 0; i < 10; i++ {
        go producer(done,q)
    }

    for j := 0; j < 5; j++ {
        go consumer(done, q, sumCh)
    }

    sumCh <- 0
    time.Sleep(2 * time.Second)

    // ANY channel can repeatedly read the ZERO VALUE of the channel's data type, from a CLOSED channel
    // after done channel is CLOSED, the producer and consumer goRoutines will be able to read from done channel (it will become non-blocking), and exit
    close(done)

    fmt.Println("Sum: ", <-sumCh)
}
