Consider 3 threads in C++.There are 3 std::atomic<int> variables, x, y, and z initialized to 0 before the threads have been created.

Answer the following 2 questions for each of the code snippets A, B, and C below :
- What are all the possible values printed by the code snippet ?
- Will your answer change if x is not atomic ? (x change to int only)

/********************************************************* Code Snippet A ********************************************************************/
Thread A | Thread B |
---------------------------------- | ------------------------------------------ |
x.store(1, memory_order_relaxed);  | while (y.load(memory_order_acquire) != 2); |
y.store(2, memory_order_release);  | cout << x.load(memory_order_relaxed);      |
---------------------------------- | ---------------------------------------- - |

1.1 A calls notepad X, asks to store 1 as part of batch 1
1.2 A calls notepad Y, asks to store 2 as LAST VALUE of batch 1
1.3 In the meantime, B keeps calling notepad Y, asking for a value of 2 WITH BATCH INFORMATION
2.  B will get it from (1.2) eventually, and Acquire-Release semantics kick in
3.  Once B gets it, he will call notepad X with ALL his batch information (batch 1), and ask for last values in ANY of the batches
4.  B therefore gets TRUE for notepad X

- Possible values printed : 1
- making variables non-atomic as no effect

atomic : either see as done or not done
visible side effects of x
- initial value of x = 0
- sees x = 1 (since it happens before due to Acquire-Release)
- if change x to int, there is no data race, so still 1
  W-W, R-W all have arrows between them, so all memory locations are ok


/********************************************************* Code Snippet B ********************************************************************/
Thread A | Thread B |
---------------------------------- | ---------------------------------------- - |
x.store(1, memory_order_relaxed);  | while (y.load(memory_order_acquire) != 2); |
y.store(2, memory_order_release);  | cout << x.load(memory_order_relaxed);      |
x.store(3, memory_order_relaxed);  | while (z.load(memory_order_acquire) != 4); |
z.store(4, memory_order_release);  | cout << x.load(memory_order_relaxed);      |
x.store(5, memory_order_relaxed);  |                                            |
---------------------------------- | ---------------------------------------- - |

x = 1 ovewritten by x = 3, x = 3 is the latest one to be written
x = 3 and x = 5 has no happens - before, so if x is non-atomic, there will be data races

1.1 A calls notepad X, asks to store 1 as part of batch 1
1.2 A calls notepad Y, asks to store 2 as LAST VALUE of batch 1
1.3 In the meantime, B keeps calling notepad Y, asking for value of 2 WITH BATCH INFORMATION
1.4 B will get it from (1.2), Acquire-Release semantics kick in
1.5 B calls notepad X with ALL his batch information (batch 1 only), and asks for last values in ANY of the batches (batch 1 only)
1.6 B can either get last value of batch 1, OR ANY OF THE LATER VALUES DOWN THE LIST
1.6 B can get 1,3,5 for X

2.1 In the meantime, A calls notepad X and asks to store 3 as part of batch 2
2.2 A then calls notepad Z and asks to store 4 as LAST VALUE of batch 2
2.3 A then calls notepad X and asks to store 5 as part of batch 3
2.4 In the meantime, B is calling notepad Z and asking for value of 4 WITH BATCH INFORMATION
2.5 B will get it from (2.2), Acquire-Release semantics kick in
2.6 B calls notepad X with ALL his batch information (batch 2), and asks for last values in ANY of the batches he knows about
2.7 B can either get last value of batch 2, OR ANY OF THE LATER VALUES DOWN THE LIST
2.8 B can get 3,5 for X
