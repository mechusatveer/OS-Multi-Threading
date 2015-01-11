In computer science, the test-and-set instruction is an instruction used to write to a memory
location and return its old value as a single atomic (i.e., non-interruptible) operation.
Typically, the value 1 is written to the memory location. If multiple processes may access the
 same memory location, and if a process is currently performing a test-and-set, no other process
 may begin another test-and-set until the first process is done.

function Lock(boolean *lock)
{
    while (test_and_set(lock) == 1);
}

The calling process obtains the lock if the old value was 0. It spins writing 1 to the variable
until this occurs.

#define LOCKED 1
 
int TestAndSet(int* lockPtr) {
    int oldValue;
 
    // Start of atomic segment
    // The following statements should be interpreted as pseudocode for
    // illustrative purposes only.
    // Traditional compilation of this code will not guarantee atomicity, the
    // use of shared memory (i.e. not-cached values), protection from compiler
    // optimization, or other required properties.
    oldValue = *lockPtr;
    *lockPtr = LOCKED;
    // End of atomic segment
 
    return oldValue;
}

Implementing mutual exclusion using test and set
================================================
volatile int lock = 0;

void Critical() {
    while (TestAndSet(&lock) == 1);
    critical section // only one process can be in this section at a time
    lock = 0 // release lock when finished with the critical section
} 
