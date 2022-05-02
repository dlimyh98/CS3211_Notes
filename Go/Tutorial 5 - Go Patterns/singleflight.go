package main

import (
	"fmt"
	"io"
	"math/rand"
	"sync/atomic"
	"time"

	"golang.org/x/sync/singleflight"
)

type db struct {
	numConn int64
}

func (d *db) Read(key string) (string, error) {
	if err := d.incConn(); err != nil {
		return "", err
	}
	defer d.decConn()

	time.Sleep(50 * time.Millisecond)
	switch key {
	case "cat":
		return "funny_cat.png", nil
	case "dog":
		return "funny_dog.png", nil
	default:
		return "", fmt.Errorf("key not found")
	}
}

func (d *db) ReadCat() (interface{}, error) {
	return d.Read("cat")
}

func (d *db) ReadDog() (interface{}, error) {
	return d.Read("dog")
}

func (d *db) incConn() error {
	for {
		n := atomic.LoadInt64(&d.numConn)
		if n > 10 {
			return fmt.Errorf("I am overloaded!")
		}
		if n > 100 {
			panic("Oh no, I give up!")
		}
		if atomic.CompareAndSwapInt64(&d.numConn, n, n+1) {
			return nil
		}
	}
}

func (d *db) decConn() {
	atomic.AddInt64(&d.numConn, -1)
}

var readerIO = io.Discard

func main() {
	db := db{}

    // singleflight is used when you have a func() that you repeatedly call, but it always returns the SAME result
    // we can then batch these results together, making it so that only ONE of these functions will run (the rest of the functions will WAIT for this to complete)
    // after this function runs, it will give it's answer to the others (hence function is NOT repeatedly invoked)
	var reqGp singleflight.Group

	wantCat := func() {
		for {
			cat, err, _ := reqGp.Do("cat", db.ReadCat)    // "cat" is the key for this singleflight group (all db.ReadCat() will be associated with same "cat" key)
			if err != nil {
				// Complain
				fmt.Println("Aww my cat!")
			}
			// Enjoy the cat photo for some time
			time.Sleep(time.Duration(rand.Intn(200)) * time.Millisecond)

			// Save the cat
			fmt.Fprintf(readerIO, cat.(string))
		}
	}

	wantDog := func() {
		for {
			dog, err, _ := reqGp.Do("dog", db.ReadDog)    // "dog" is the key for this singleflight group (all db.ReadDog() will be associated with the same "dog" key)
			if err != nil {
				// Complain
				fmt.Println("Aww my dog!")
			}
			// Enjoy the dog photo for some time
			time.Sleep(time.Duration(rand.Intn(300)) * time.Millisecond)

			// Save the cat
			fmt.Fprintf(readerIO, dog.(string))
		}
	}

	fmt.Println("At first, 5 want cat and 5 want dog")
	for i := 0; i < 5; i++ {
		go wantCat()
	}
	for i := 0; i < 5; i++ {
		go wantDog()
	}

	time.Sleep(3 * time.Second)
	fmt.Println("The pics have gone viral, 1000 want cat and 1600 want dog")
	for i := 0; i < 995; i++ {
		go wantCat()
	}
	for i := 0; i < 1595; i++ {
		go wantDog()
	}

	time.Sleep(3 * time.Second)
}
