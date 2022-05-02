/********************************** Atomically Reference Counted (ARC) *****************************/
// Arc only allows SHARED references (which cannot be mutated)
// Arc owns the object, no other way to manipulate it

let v = Arc:: new(!vec[1,2]);    // reference count = 1
let v2 = v.clone();              // reference count = 2

thread::spawn(move || {
    println!("{}", v.len());    // give one of the references to this thread
});

// vector only destroyed when Arc reference count reaches 0


/********************************** Mutex *****************************/
// Shared memory paradigm
// Mutex owns the data (unlike C++, Mutex cannot be locked/unlocked for usage)

fn sync_inc(counter: &Mutex<i32>) {
    let mut data Guard<i32> = counter.lock();  // lock returns Guard (if successful), a proxy to access data
    *data += 1;                                // use Guard to access data
}


/********************************** MPSC (Multiple Producer Single Consumer) *****************************/
// Message passing paradigm
// Channel (like Go), that accepts one reader and multiple writers

let (tx,rx) = mpsc::channel();
let tx2 = tx.clone();

thread::spawn(move || tx.send(5));
thread::spawn(move || tx2.send(4));

// prints 4 and 5 in unspecified order, since no coordination between threads
println!("{:?}", rx.recv());
println!("{:?}", rx.recv());
