func main() {
    var wg sync.WaitGroup    // declare variable named "wg" of type sync.WaitGroup

    // for loop, assign "hello", "greetings" or "good day" to salutation
    for _, salutation := range []string{"hello", "greetings", "good day"} {
        wg.Add(1)            // add 1 goRoutines to wait

        // goRoutine (NOT a normal function)
        // goRoutine is running a CLOSURE that has closed over the iteration variable "salutation"
        go func() {
            defer wg.Done()           // defers wg.Done() from executing, until surrounding function returns (the goRoutine func())
                                      // wg.Done() will decrement the WaitGroup counter by 1
            fmt.Println(salutation)
        }()                           // no arguments to goRoutine
    }

    wg.Wait()                         // wait for all goRoutines to finish (when the WaitGroup counter decrements to 0)
}

// This main() function is likely to only print x3 "good day"
// This is because the closure only starts running AFTER the forLoop ends, which means salutation is likely to be "good day"
// if you pass a copy of salutation into closure instead...

go func(salutation string) {
    defer wg.Done()
    fmt.Println(salutation)
} (salutation)
// now each closure gets a copy of salutation AT THE TIME when goRoutine is STARTED
// likely to print "hello" , "greetings" , "good day" if run x3 now
