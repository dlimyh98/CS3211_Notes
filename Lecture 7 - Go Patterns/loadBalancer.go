/************************************************ REQUESTS ***********************************************************/
type Request struct {
    fn func() int   // function (returning int) that Request has to complete
    c chan int      // channel of integers INSIDE the request, so that the worker can return the result to the requester DIRECTLY
}

// Requester = sends Requests to the Balancer
func requester(work chan<- Request) {               // input = channel of requests
    c := make(chan int)
    for {
        Sleep(rand.Int63n(nWorker * 2 * second))    // kill some time (fake load)
        work <- Request{workFn, c}                  // requests are placed on work channel, which is later read by Balancer

        // Once the work is processed, the result will be read from the channel c that is PART OF the request
        result := <-c
        furtherProcess(result)
    }
}

/************************************************ WORKER ***********************************************************/
type Worker struct {
    requests chan Request    // channel of Requests FOR EACH WORKER
    pending int              // number of pending tasks FOR EACH WORKER (internal counter to keep track of how loaded the system is)
    index int                // keeps track of INDIVIDUAL worker's position in the heap
}

// (w *Worker) = receiver function
//  - work() function can RECEIVE a Worker pointer
//  - persists when we return to the caller scope, since we received a POINTER type
//  - call using w.work()
func (w *Worker) work(done chan *Worker) {
    for {
        req := <-w.requests    // worker takes whatever work is on the IT'S OWN request channel

        // worker sends back result of execution DIRECTLY to requester (requester c in this case)
        req.c <- req.fn()      // processes the request based on whatever the function is

        // done channel to keep track of what work has been done by worker, so that balancer can keep track of how loaded the system is
        // how many requests are executing, and when a request is done, decrease the load from the system
        done <- w              // notifies the balancer that the work is done
    }
}

/************************************************ BALANCER ***********************************************************/
type Pool []*Worker      // slice of Worker pointers

type Balancer struct {
    pool Pool           // pool of Workers (implemented as a Heap)
    done chan *Worker   // done channel where workers can say they have completed their work
}

// pool of Workers implementation
func (p Pool) Less(i, j int) bool {
    // number of pending tasks of a worker is used to sort the workerHeap
    // less loaded workers will be at the top of Heap
    return p[i].pending < p[j].pending
}

func (b *Balancer) balance(work chan Request) {
    for {
        select {
        case req := <-work:     // Balancer received a request
            b.dispatch(req)     // .... so send it to any Worker
        case w := <-b.done      // some Worker has finished
        b.completed(w)          // .... so update its info
        }
    }
}

// Sending a Job out
func (b *Balancer) dispatch(req Request) {
    w := heap.Pop(&b.pool).(*Worker)     // Grab the least loaded worker
    w.requests <- req                    // send request to the selected worker
    w.pending++                          // selected worker will have one more pending in it's work queue
    heap.Push(&b.pool,w)                 // return the worker back to heap (where it will be sorted accordingly)
}

// a Job is completed (the worker updated the Balancer that the job is done by writing to the done channel...)
func (b *Balancer) completed(w *Worker) {
    w.pending--                          // selected worker will have one less pending in it's work queue
    heap.Remove(&b.pool, w.index)        // remove it from the heap
    heap.Push(&b.pool, w)                // insert it back into the heap (where it will be sorted)
}
