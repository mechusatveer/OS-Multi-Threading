http://stackoverflow.com/questions/800383/what-is-the-difference-between-mutex-and-critical-section

For Windows, critical sections are lighter-weight than mutexes.

Mutexes can be shared between processes, but always result in a system call to the kernel 
which has some overhead.

Critical sections can only be used within one process, but have the advantage that they only 
switch to kernel mode in the case of contention - Uncontended acquires, which should be the 
common case, are incredibly fast. In the case of contention, they enter the kernel to wait on
some synchronization primitive (like an event or semaphore).

I wrote a quick sample app that compares the time between the two of them. On my system for 
1,000,000 uncontended acquires and releases, a mutex takes over one second. A critical section 
takes ~50 ms for 1,000,000 acquires.

Here's the test code, I ran this and got similar results if mutex is first or second, so we aren't
seeing any other effects.

HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
CRITICAL_SECTION critSec;
InitializeCriticalSection(&critSec);

LARGE_INTEGER freq;
QueryPerformanceFrequency(&freq);
LARGE_INTEGER start, end;

// Force code into memory, so we don't see any effects of paging.
EnterCriticalSection(&critSec);
LeaveCriticalSection(&critSec);
QueryPerformanceCounter(&start);
for (int i = 0; i < 1000000; i++)
{
    EnterCriticalSection(&critSec);
    LeaveCriticalSection(&critSec);
}

QueryPerformanceCounter(&end);

int totalTimeCS = (int)((end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart);

// Force code into memory, so we don't see any effects of paging.
WaitForSingleObject(mutex, INFINITE);
ReleaseMutex(mutex);

QueryPerformanceCounter(&start);
for (int i = 0; i < 1000000; i++)
{
    WaitForSingleObject(mutex, INFINITE);
    ReleaseMutex(mutex);
}

QueryPerformanceCounter(&end);

int totalTime = (int)((end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart);

printf("Mutex: %d CritSec: %d\n", totalTime, totalTimeCS);
