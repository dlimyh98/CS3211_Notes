// Go Functions may be closures
// Closure is a function value that references variables from OUTSIDE it's body
// Function may access and assign to the referenced variable, in a sense this function is "bound" to the variables


// intSeq() function, no parameters, returnValue = int
func intSeq() func() int {
    i := 0

    // returns an anonymous function defined below
    // returned function closes over the variable i to form a CLOSURE
    return func() int {
        i++
        return i
    }

}

func main() {
    // call intSeq, assigning the result (a FUNCTION) to nextInt
    // This function value captures IT'S OWN i value, which will be updated each time we call nextInt()
    nextInt := intSeq()

    fmt.Println(nextInt())
    fmt.Println(nextInt())
    fmt.Println(nextInt())

    // State is unique to that PARTICULAR function
    newInts := intSeq()
    fmt.Println(nextInt())
}

/* Output */
// 1
// 2
// 3
// 1
