func main() {
    startTime := time.Now()
    g := new(errgroup.Group)

    // ranging over SLICE that contains function (which return errors)
    for _,fn := range[]func() error {
        func() error {
            time.Sleep(time.Second)
            return nil
        },
        func() error {
            return fmt.Errorf("Error!")
        },
    } {
        // normally you would call the goRoutine by go fn() {.....}
        // but since we are using errgroup now, we replaced it with errgroup.Group.Go(of function), which returns the FIRST error it gets
        g.Go(fn)
    }

    // .Wait() blocks until all function calls from the Go method have returned, then returns the FIRST non-nil error (if any) from them
    // inline declaration of err, then immediately checking if err is nill
    if err := g.Wait(); err == nill {
        fmt.Println("No error")
    } else {
        fmt.Println("Error thrown:", err)
    }

    fmt.Println("Time elapsed:", time.Now().sub(startTime))
}
