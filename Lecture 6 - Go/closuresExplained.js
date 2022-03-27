outer = function () {
    var a = 1;
    
    var inner = function() {
        console.log(a);
    }

    return inner;   // returns a function
}

var fnc = outer(); // execue outer() to get inner()
fnc();             // prints 1


/* 
 * 1.1  inner() gains access to all outer() LOCAL variables (including a)
 * 1.2  var a is IN SCOPE for inner()
 * 
 * 2.1  Normally when a function exits (in C or C++), all of it's local variables are blown away
 * 2.2  In JS, if we return the inner function and assign it to a variable fnc so that it PERSISTS after outer has exited,
 *      all of the variables that were in scope when inner() was defined ALSO persist. 
 *      var a has been closed over (CLOSURE formed)
 * 
 * 2.3  var a is totally PRIVATE to fnc
 *
 * 3.1  a belongs to the scope of outer()
 * 3.2  scope of inner() has parent pointer to scope of outer()
 * 3.3  fnc is a variable which points to inner()
 * 3.4  a persists as long as fnc persists
 *
 * */
