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
	inputCh  <-chan Event   // Worker reads inputs from inputCh (channel for receiving)
	outputCh chan<- Event   // Worker sends results to outputCh (channel for sending)
}

// starts a newWorker, accepting inputs (inputCh, channel of events outputCh),
// returns POINTER to newWorker
func newWorker(inputCh, outputCh chan Event) *worker {
	return &worker{
		inputCh:  inputCh,
		outputCh: outputCh,
	}
}

// Receiver function (can receive pointer to worker)
// the pointer to worker can then call the start function, similar to OOP class
func (w *worker) start(
	done <-chan struct{},
	fn EventFunc, wg *sync.WaitGroup,
) {
	go func() {
		defer wg.Done()
		for {
			select {
			case e, more := <-w.inputCh:    // Read from the worker's inputCh
				if !more {                  // more indicates whether received value is sent by channel (TRUE), or is zero value returned due to closed channel (FALSE)
					return
				}
				select {
				case w.outputCh <- fn(e):   // do work in fn(e), then write results to outputCh
				case <-done:
					return
				}
			case <-done:
				return
			}
		}
	}()
}

// generates work for us to do
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
	outputCh := make(chan Event, 1)
	inputCh := genEventsCh()

	// Fan-out the stream of input to multiple workers
	var wg sync.WaitGroup
	for i := 0; i < 10; i++ {
		wg.Add(1)
		newWorker(inputCh, outputCh).                 // sending jobs from inputCh to individual worker is synchronous
			start(done, func(e Event) Event {         // but all the workers can process their intputs concurrently
				time.Sleep(e.procTime)
				return e
			}, &wg)
	}

	readerDone := make(chan struct{})
	go func() {
		for output := range outputCh {
			fmt.Printf("Event id: %d\n", output.id)
		}
		close(readerDone)
	}()

	wg.Wait()

	// Close outputCh and wait for reader to finish reading
    // Result is NOT SYNCHRONOUS, due to variable processing time between different workers and that the workers of NOT aware of each other's completion
	close(outputCh)
	<-readerDone
}
