/************************* Ownership (does not allow Aliasing, can't point with two pointers to the same data) ***************/
fn main() {
    let mut book = Vec::new();    // mutable vector initialized, owner is main()

    book.push(...);
    book.push(...);

    // copy over the fields from stack, forget about first book in main()
    // this is unlike C++, which does a deep copy of book
    // Rust hands ownership of book to function, which is somewhat similar to C++ move(), but enforced at compilation time and no ownership is retained
    // Rust can do deep copy using clone()
    publish(book);

    // Compilation error = use of moved value book
    publish(book);
}

fn publish(book: Vec<String>) {
    // some code ....
    // runs destructor for book when function exits
}





/************************* Shared Borrow (allows Aliasing, but no Mutations) ***************/
// let some function borrow some reference for some time, do non-mutable actions on it, and then return

fn main() {
    let mut book = Vec::new();    // mutable vector initialized, owner is main()

    book.push(...);
    book.push(...);

    // shared borrow the vector, creating NEW reference to the same vector
    // function that borrows reference cannot modify the reference (function only READS, no modification)
    publish(&book);
    publish(&book);
}

fn publish(book: &Vec<String>) {
    // some code ....
    // runs destructor for book when function exits
}





/************************* Mutable Borrow (no Aliasing, but Mutation allowed) ***************/
// let some function borrow some reference for some time, do mutable actions on it, and then return

fn main() {
    let mut book = Vec::new();    // mutable vector initialized, owner is main()

    book.push(...);
    book.push(...);

    // while this reference is borrowed in a MUTABLE way, no other function can read/write what is being referenced
    // i.e. initial book is locked (it cannot be mutated/accessed in anyway)
    publish(&book);
    publish(&book);
}

fn publish(book: &Vec<String>) {
    // some code ....
    // runs destructor for book when function exits
}
