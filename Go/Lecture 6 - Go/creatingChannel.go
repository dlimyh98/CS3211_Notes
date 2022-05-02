/********************** Declare and create BIDIRECTIONAL channel ***************************/
var dataStream chan Interface{}
dataStream = make(chan interface{})

/********************** Declare and create UNIDIRECTIONAL channel ***************************/
var receiveChan <-chan interface{}
var sendChan chan<- interface{}

dataStream = make(chan interface{})
receiveChan = dataStream
sendChan = dataStream

/* by default, SENDS and RECEIVES block until both sender and receiver are ready */
timerChan := make(chan time.Time)

go func() {
    time.Sleep(deltaT)
    timeChan <- time.Now()
}()

completedAt := <-timerChan    // receive here blocks until timerChan delivers
