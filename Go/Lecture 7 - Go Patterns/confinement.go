// Confinement = achieve safe operation via...
// 1. Synchronization primitives for sharing memmory (e.g. syncMutex)
// 2. Synchronization via communication (e.g. Channels)

// Two types of Confinement
// 1. Ad-Hoc confinement: Data is MODIFIED only from ONE goRoutine, even though it is accessible from MULTIPLE goRoutines
// 2. Lexical confinement: Restrict access to the SHARED locations

/************************************** Examples of Lexical Confinement *****************************************/

// Expose only the reading/writing handle of a Channel (which may be unidirectional)
chanOwner := func() int {
    results := make(chan int, 5)
    go func() {
        defer close(results)
        for i := 0; i <= 5; i++ {
            results <- i            // only the writing handle is exposed
        }
    }()

    return results
}

// Exposing only a slice of the array (but may cause cache line overlap)
printData := func(wg *sync.WaitGroup, data []byte) {
    defer wg.Done()
    var buff bytes.Buffer
    for _, b := range data {
        fmt.Fprintf(&buff, "%c", b)
    }
    fmt.Println(buff.String())
}

var wg sync.WaitGroup
wg.Add(2)
data := []byte("Golang")
go printData(&wg, data[:3])    // expose only slice of array
go printData(&wg, data[3:])    // expose only slice of array
wg.Wait()
