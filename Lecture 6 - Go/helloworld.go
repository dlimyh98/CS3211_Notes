package main
import "fmt"

func main() {
    go sayHello()
    // continue doing other things
}


// Normal Function
func sayHello() {
    // print might never happen, since main goroutine finishes execution before sayHello completes
    fmt.Println("hello")
}

// Anonymous Function
go func() {
    fmt.Println("hello")
}
