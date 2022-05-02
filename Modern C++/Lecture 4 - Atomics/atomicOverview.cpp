/* Memory Model at the language level */

/* std::memory_order enumeration */
/* - Default (STRONGEST) ordering - std::memory_order_seq_cst
*  - Store Operations can have...
*    1. memory_order_relaxed
*    2. memory_order_release
*    3. memory_order_seq_cst
* 
*  - Load Operations can have...
*    1. memory_order_consume
*    2. memory_order_acquire
*    3. memory_order_seq_cst
* 
*  - Read-Modify-Write Operations can have
*    1. memory_order_consume
*    2. memory_order_acquire
*    3. memory_order_release
*    4. memory_order_acq_rel
*    5. memory_order_seq_cst
*/

/* Memory Ordering for ATOMIC operations */
/* 1. Sequentially Consistent ordering 
*     - memory_order_seq_cst
* 
*  2. Acquire-Release ordering
*     - memory_order_acquire
*     - memory_order_release
*     - memory_order_acq_rel
*     - memory_order_consume
* 
*  3. Relaxed ordering
*     - memory_order_relaxed
*/
