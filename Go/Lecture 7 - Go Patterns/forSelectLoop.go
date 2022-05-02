//  For-Select loops are used to ...
//  1. Sending iteration variables out on a channel
//  2. Looping and Waiting to be stopped

func consumer(done chan struct{}, q queue, sumCh chan int) {
    for {
        select {
        case <-done:               // often a done channel is included
            return                 // if can read from done channel, we must stop
        default:                   // else we can continue doing our other work
        }

        num, ok := try_dequeue()
        if ok {
            sumCh <- num + <- sumCh
        }
    }
}
