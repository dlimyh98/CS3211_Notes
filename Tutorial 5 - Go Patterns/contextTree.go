import (
    "context"
    "fmt"
    "os"
    "os/signal"
    "syscall"
    "time"
)

func handleSigs() chan struct{} {
    done := make(chan struct{})

    go func() {
        sigs := make(chan os.Signal, 1)                      // make a channel of osSignals, with buffered capacity of 1
        signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM) // whenever syscall.SIGINT and syscall.SIGTERM is fed to goRoutine, it will be relayed to sigs channel
        <-sigs                                               // BLOCKING read from sigs channel
        close(done)                                          // if manage to read from sigs channel (SIGINT or SIGTERM has been fed), signal to goRoutines to stop (by closing done channel)
    }()

    return done
}

var (
    timeout1 = 4 * time.Second
    timeout2 = 2 * time.Second
)

func main() {
    startTime := timeNow()
    //.Background() returns a non-nil, EMPTY context. Typically used as TOP-LEVEL Context for incoming requests
    // context.WithTimeout take in Context (the parent), and return a derived Context (the child) and a CancelFunc
    // context also has a done channel, that's closed when work done on behalf of this context should be cancelled

    // created from .Background context, with timeout of 4s
    // if this ctx exits, it closes ALL CHILD contexts under it
    ctx, _ := context.WithTimeout(context.Background(), timeout1)

    // created from ctx, with timeout of 2s
    // should stop if PARENT ctx closes OR after 2s
    ct2, _ := context.WithTimeout(ctx, timeout2)
    go func() {
        <-ctx2.Done()
        fmt.Printf("ctx2 done at %v\n", time.Now(),Sub(startTime))
    }()

    // created from ctx, with CancelFunc
    // should stop if PARENT ctx closes OR if SIGINT/SIGTERM received (as CancelFunc will then be called)
    // WithCancel returns a copy of parent with a new Done channel. 
    // Returned context's Done channel is CLOSED when the returned cancel function is called, or when the parent context's Done channel is closed, whichever happens first.
    ctx3, cancel := context.WithCancel(ctx)
    go func() {
        <-ctx3.Done()
        fmt.Printf("ctx3 done at %v\n", time.Now(),Sub(startTime))
    }()

    // KEY IDEA : if Parent context closes, then ALL child contexts must closed
    // but a Child context can close without affecting it's Parent context, or another fellow Child context
    go func() {
        <-handleSigs()         // when handleSigs() closes and returns a CLOSED done channel, only then will this read be completed
        cancel()               // calling ctx3 CancelFunc()
        fm.Printf("signal in at %v\n", time.Now().Sub(startTime))
    }()

    // ctx.Done() will only be read from successfully once 4 seconds or more have passed OR some cancellation is done
    <-ctx.Done()

    fmt.Printf("ctx %v\n", time.Now().Sub(startTime))
}
