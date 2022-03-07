#include <atomic>
std::atomic<int> a;
int b;


/******************** Happens-before is always true WIHIN a thread, as long as visible side effects remain similar ***********************/
void foo1() {
	a.store(1, std::memory_order_relaxed);
	b = 1;
	// side effect after foo1() terminates ---> a=1, b=1
}

void foo2() {
	b = 1;
	a.store(1, std::memory_order_relaxed);
	// side effect after foo2() terminates ---> a=1, b=1
}

// since foo1 and foo2 have the SAME VISIBLE SIDE EFFECTS, the internals of the thread CAN BE reordered (Happens-before is NOT violated)
// thus, compiler is free to choose between foo1() and foo2(), they are equivalent

void foo3() {
	a.store(2, std::memory_order_relaxed);
	a = 1;
}

// foo3()'s internals CANNOT be reordered, since it would affect the visible side effects of foo3()



/****************************** Accesses to single atomic variable from same thread CANNOT be reordered ***********************************/
std::atomic<int> c{ 0 };
t1: stdcout << ++c (i) << ++c (ii);
t2: stdcerr << ++c(iii)

// Valid Orderings (later access by the same thread, never get an earlier value)
// even if all operations are done seq_cst, there are still different options that ensure a consistent GLOBAL view among all threads.
i ii iii
cout : 1 2
cerr : 3

i iii ii
cout : 1 3
cerr 2

iii i ii
cout : 2 3
cerr : 1

// Invalid Orderings (later access by the same thread, get an earlier value)
ii i iii
cout : 2 1
cerr : 3



/********************************** Atomic Ordering is less strict than Sequential Ordering *******************************************/
relaxed - no constraints.
consume - no loads that are dependent on the newly loaded value can be reordered wrt.the atomic load.I.e. if they are after the atomic load in the source code, they will happen after the atomic load too.
acquire - no loads can be reordered wrt.the atomic load.I.e. if they are after the atomic load in the source code, they will happen after the atomic load too.
release - no stores can be reordered wrt.the atomic store.I.e. if they are before the atomic store in the source code, they will happen before the atomic store too.
acq_rel - acquire and release combined.
seq_cst - Basically, all other orderings only ensure that specific disallowed reorderings don't happen only for the threads that consume/release the same atomic variable.
          Memory accesses can still propagate to other threads in any order. This ordering ensures that this doesn't happen(thus sequential consistency).

    `
/********************************** Acquire - Release Semantics ********************************************/
std::atomic<int> a{ 0 };
int b = 0;

t1: b = 1; a.store(1, release);
t2: cout << a.load(acquire) << b;
// Acquire-Release semantics, but it is not guaranteed in which order they are run in

// Possibility 1 (a.store THEN a.load)
1. b = 1 happens before a.store(), due to Program Order
2. cout << a.load() happens before cout << b, due to Program Order
3. a.load() reads the value provided by a.store() (all writes in OTHER threads that release the SAME atomic variable are visible in CURRENT thread)
cout: 1 1

// Possibility 2 (a.load THEN a.store)
1. cout << a.load() happens before cout << b, due to Program Order
2. but a.load() has no a.store() to synchronize with, no writes in other threads on a have been done yet
3. Hence, cout << a.load() reads the global value 0 (could potentially have data race as well!)
4. Since b is not synchronized, there is data race
cout: 0 ?

// So how do we ensure that the store comes before the load? We check if the store has actually happened yet!
better t2:
    if (a.load(acquire) == 1) {  
        // we know that the a.store() has run first
        cout << 1 << b;
    }
    else {
        cout << 1 << "this could cause a data race, so let's not read b";
    }
