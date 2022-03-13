package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

type Event struct {
	id       int64
	procTime time.Duration
}

type EventFunc func(Event) Event

type worker struct {
	inputCh  chan Event
	outputCh chan Event
}

func newWorker() *worker {
	return &worker{
		inputCh:  make(chan Event),
		outputCh: make(chan Event),
	}
}

func (w *worker) start(
	done chan struct{},
	fn EventFunc, workerQueue chan *worker, wg *sync.WaitGroup,
) {
	go func() {
		defer func() {
			close(w.outputCh)
			wg.Done()
		}()
		for {
			select {
			case workerQueue <- w: // <-- change: sign up for work
				select {
				case e := <-w.inputCh:
					select {
					case w.outputCh <- fn(e):
					case <-done:
						return
					}
				case <-done:
					return
				}
			case <-done:
				return
			}
		}
	}()
}

func orderedMux(
	done chan struct{},
	inputCh chan Event, workerQueue chan *worker, workerOutputCh chan chan Event,
) {
	go func() {
		for {
			select {
			case e, more := <-inputCh:
				if !more {
					return
				}
				select {
				case w := <-workerQueue:
					select {
					case workerOutputCh <- w.outputCh:
					case <-done:
						return
					}
					w.inputCh <- e
				case <-done:
				}
			case <-done:
				return
			}
		}
	}()
}

func genEventsCh() chan Event {
	outputCh := make(chan Event)
	go func() {
		counter := int64(1)
		rand.Seed(time.Now().Unix())
		for i := 0; i < 30; i++ {
			outputCh <- Event{
				id:       counter,
				procTime: time.Duration(rand.Intn(100)) * time.Millisecond,
			}
			counter++
		}
		close(outputCh)
	}()
	return outputCh
}

func main() {
	done := make(chan struct{})
	workerQueue := make(chan *worker)
	workerOutputCh := make(chan chan Event)
	inputCh := genEventsCh()
	orderedMux(done, inputCh, workerQueue, workerOutputCh)

    // 1.  whenever a worker is free, it will send itself to workerQueue
    // 2.  orderedMux will send the jobs to the available worker's inputCh (every worker has it's OWN inputCh and outputCh)
    // 3.  once the individual worker is done, it will submit IT'S OWN outputCh to workerOutputCh (via orderedMux)
    // 3.1 Hence when a worker's outputCh appears in workerOutputCh, it is the future
    // 3.2 When main() reads from workerOutputCh, it is fufilling the promise

	var wg sync.WaitGroup
	for i := 0; i < 10; i++ {
		wg.Add(1)
		newWorker().start(done, func(e Event) Event {
			time.Sleep(e.procTime)
			return e
		}, workerQueue, &wg)
	}

	readerDone := make(chan struct{})
	go func() {
        // reads from workerOutputCh, which then reads from the selected individual outputCh ITSELF
		for outputCh := range workerOutputCh { // <-- reading a stream of promises

            // if the promise has not been fufilled yet, will block here
			output, more := <-outputCh // <-- read from the promise once only
			if !more {
				break // <-- a worker breaks its promise; workers exiting
			}
			fmt.Printf("Event id: %d\n", output.id)
		}
		close(readerDone)
	}()

	time.Sleep(2 * time.Second)

	// stop all workers and wait for them to exit
	close(done)
	wg.Wait()

	close(workerOutputCh)
	<-readerDone
    // Output will be synchronous
}
