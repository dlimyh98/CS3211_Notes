type Result struct {
    Error error
    Response *http.Response
}

checkStatus := func(done <-chan interface{}), urls ...string) <-chan Result {
    results := make(chan Result)
    go func() {
        defer close(results)
        for _, url := range urls {
            var result Result
            resp, err := http.Get(url)
            result = Result{Error : err, Response: resp}
            select {
            case <-done:
                return
                case results <- result:
            }
        }
    }()

    return results
}

// error handling is restricted to the goRoutine that HANDLES the result
done := make(chan interface{})
defer close(done)
urls := []string{"https://www.google.com, "https://badhost"}

for result := range checkstatus(done, urls...) {
    if result.Error != nil {
        fmt.Printf("Error: %v", result.Error)
        continue
    }

    fmt.Printf("Response: %v\n", result.Response.Status)
}
