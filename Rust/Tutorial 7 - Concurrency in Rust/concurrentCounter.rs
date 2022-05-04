/******************** C++ Concurrent Counter ********************/
int counter;

int main() {
    std::thread t0{[] () {++counter;}};
    std::thread t1{[] () {++counter;}};

    t0.join();
    t1.join();

    std::cout << counter << std::endl;
    return 0;
}

/******************** Direct Port to Rust (Naive Attempt) ********************/
use std::thread;

fn main() {
    let mut counter = 0;

    // captures REFERENCE to counter, via Closure
    let t0 = thread::spawn(|| {counter += 1;});
    let t1 = thread::spawn(|| {counter += 1;});

    t0.join();
    t1.join();

    println!("{}", counter);
}

// Issue 1 - counter is being shared AND mutated simultaneously
// PROBLEM:  two threads capture a REFERENCE to counter (borrowing), and then mutate it
//
// FIX:      use Mutex<T> to allow mutation for data shared (only one thread mutates any one time)


// Issue 2 - counter does not live long enough
// PROBLEM: for Rust, the data captured by closure must live at least as long as static lifetime.
//                    since you are not required to join a thread in Rust, it is possible that
//                    thread lives forever. Hence, data reference by thread must live "forever"
//
// SOLUTION 1: make sure that counter does not get freed before all threads exit.
//             use Arc<T>, which allocates data on the heap, tracks reference counter internally, and
//             only frees the allocation when there are no more references.
//
// SOLUTION 2: use scoped thread - it will join all threads before main ends, allowing threads to
//             take closures that do not live for 'static


/******************** Direct Port to Rust (Proper) ********************/
use std::thread;
use std::sync::{Mutex,Arc};

fn main() {

    // want to share data and mutex across threads, so use Arc<Mutex<i32>>
    let counter = Arc::new(Mutex::new(0));

    // each thread must make a NEW Arc instance (using clone(), which does a deep copy).
    // this creates a NEW Arc instance that points to the SAME heap allocation, incrementing reference count (by right clone() does deep copy)
    // if we were to capture reference (&), lifetime issue will persist

    // UNIDIOMATIC RUST
    let t0 = {
        let counter = counter.clone();    // each thread gets it's own Arc instance

        thread::spawn(move || {
            // bring Deref and DerefMut traits into scope, to use trait methods Deref and DerefMut
            // which is used by * operator
            use std::ops::{Deref, DerefMut};

            // dereference counter to get &Mutex<i32>
            let mutex: &Mutex<i32> = counter.deref();

            // call lock on &Mutex<i32> to get Result<MutexGuard<i32>, PoisonError<...>>
            // PoisonError because if thread panics while holding mutex, then data will be likely left in inconsistent state
            let lock_result = mutex.lock();

            // unrap the result to get MutexGuard<i32>
            let mut lock_guard = lock_result.unwrap();

            // deref_mut the MutexGuard<i32> to get &mut i32, which we can use to access data
            let counter_ref: &mut i32 = lock_guard.deref_mut();
            *counter-ref += 1;
        })
    };

    // IDIOMATIC RUST
    let t1 = {
        let counter = counter.clone();
        
        thread::spawn(move || {
            *counter.lock().unwrap() += 1;
        })
    };

    t0.join().unwrap();
    t1.join().unwrap();
}
