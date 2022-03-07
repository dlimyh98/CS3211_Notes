/********** Demo 1.1 (local variables) **********/
int main() {
	int x = 5;       // A
	int y = x + 1;   // B
	return y;
}
// A sets x to 5
// Since A precedes B in Program Order (same thread), this is visible to B
// However, side effect is not necessarily implemented as actual store to memory

// C++ has the weakest possible memory model to accomadate for both Intel and ARM instruction sets
// Since there is only one thread, C++ follows the as-if rule
// C++ compiler is only obligated to EMULATE the OBSERVABLE behaviour of program
// - C++ is allowed to reorder the internals of the thread, since it does not violate the OBSERVABLE behaviour
// Hence, C++ compiler has reordered the internals of thread below, AS-IF the code was written in the original way
main:
mov     eax, 6    // (int x = 5) and (int y = x + 1) writes have dissapeared
ret               // only a write of 6 to eax register remains





/********** Demo 1.2 (global variables) **********/
int A, B;        // variables are global now, instead of local in Demo 1.1

void foo() {
	A = B + 1;
	B = 0;
}

// clang Compiler
foo() :
    mov     eax, dword ptr[rip + B]     // move B to eax register
    add     eax, 1                      // add 1 to eax register
    mov     dword ptr[rip + A], eax     // move eax to A
    mov     dword ptr[rip + B], 0       // move 0 to B
    ret
    main :

// gcc Compiler
foo() :
    mov     eax, DWORD PTR B[rip]      // move B to eax register
    mov     DWORD PTR B[rip], 0        // move 0 to B
    add     eax, 1                     // add 1 to eax register
    mov     DWORD PTR A[rip], eax      // move eax to A
    ret
    main :

// both compilations are allowed, as they do not change the VISIBLE side effects of the program
// 1. for code on the SAME thread, it will only run BEFORE/AFTER foo() will observe the same behaviour anyways
// 2. for code on DIFFERENT thread, it is either not synchronized with A/B, or will only synchronize with both of them at the same time
//    (e.g. if a mutex is locked/unlock after foo() is complete
// hence from C++ perspective, both EXECUTION ORDERS result in same OBSERVABLE behavior, and compiler is free to choose either one




/********** Demo 1.3 (atomic variables) **********/
#include <atomic>

std::atomic<int> A, B;

void foo() {
    A = B + 1;
    B = 0;
}

// foo() is similar to bar()

void bar() {
    A.store(B.load(std::memory_order_seq_cst) + 1,
        std::memory_order_seq_cst);
    B.store(0, std::memory_order_seq_cst);
}

// clang and gcc Compiler outputs now both agree, but expensive XCHG instruction is used
foo() :
    mov     eax, DWORD PTR B[rip]
    add     eax, 1
    xchg    eax, DWORD PTR A[rip]
    xor eax, eax
    xchg    eax, DWORD PTR B[rip]
    ret
