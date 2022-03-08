// user-defined type "queue", of chan int
type queue chan int

func (q queue) try_enqueue(num int) bool {
    select {
    case q <- num:
        return true
    default:              // provides exit for goRoutine when all cases are blocked (makes access asynchronous/non-blocking)
    }

    return false
}

func (q queue) try_dequeue () (int,bool) {
    select {
    case num := <-q:
        return num, true
    default:              // provides exit for goRoutine when all cases are blocked (makes access asynchronous/non-blocking)
    }

    return 0, false
}

func producer(done chan struct{}, q queue) {
    for {
        select {
        case <-done:
            return
        default:
        }

        if ok := q.try_enqueue(1); !ok {    // flip the ok boolean
            // do something
        } else {
            // do something
        }
    }
}

func consumer(done chan struct{}, q queue, sumCh chan int) {
    for {
        select {
        case <-done:
            return
        default:
        }

        num, ok := q.try_dequeue()
        if ok {
            sumCh <- num + <- sumCh         // read from sumCh channel, add num to it, then write back to sumCh channel
        }
    }
}
